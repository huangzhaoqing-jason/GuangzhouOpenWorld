#include "Game/AccountLogin/GZAccountLoginManager.h"
#include "GuangzhouOpenWorld.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/Guid.h"
#include "Misc/SecureHash.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonReader.h"
#include "Dom/JsonObject.h"
#include "HAL/PlatformProcess.h"
#include "TimerManager.h"
#include "Engine/World.h"

UGZAccountLoginManager::UGZAccountLoginManager()
{
	AccountsFilePath = FPaths::ProjectSavedDir() / TEXT("PlayerAccounts.json");
	LoadAccountData();
}

// ===== Validation =====
bool UGZAccountLoginManager::ValidateEmail(const FString& Email) const
{
	if (Email.IsEmpty()) return false;
	int32 AtPos = -1;
	if (!Email.FindChar('@', AtPos)) return false;
	int32 DotPos = -1;
	if (!Email.FindLastChar('.', DotPos)) return false;
	if (AtPos < 1 || DotPos < AtPos + 2 || DotPos >= Email.Len() - 2) return false;

	FString Domain = Email.RightChop(AtPos + 1).ToLower();
	TArray<FString> ValidDomains = { TEXT("qq.com"), TEXT("163.com"), TEXT("126.com"), TEXT("gmail.com"), TEXT("outlook.com"), TEXT("icloud.com"), TEXT("hotmail.com"), TEXT("foxmail.com"), TEXT("sina.com"), TEXT("sohu.com") };
	for (const FString& D : ValidDomains)
	{
		if (Domain == D) return true;
	}
	return true;
}

bool UGZAccountLoginManager::ValidatePhone(const FString& Phone) const
{
	if (Phone.Len() < 7 || Phone.Len() > 15) return false;
	for (int32 i = 0; i < Phone.Len(); i++)
	{
		if (i == 0 && Phone[i] == '+') continue;
		if (!FChar::IsDigit(Phone[i])) return false;
	}
	if (Phone.Len() == 11 && Phone[0] == '1')
	{
		TCHAR Second = Phone[1];
		if (Second < '3' || Second > '9') return false;
	}
	return true;
}

bool UGZAccountLoginManager::ValidatePassword(const FString& Password) const
{
	if (Password.Len() < 8 || Password.Len() > 16) return false;
	bool bHasLetter = false, bHasDigit = false;
	for (const TCHAR& C : Password)
	{
		if (FChar::IsAlpha(C)) bHasLetter = true;
		if (FChar::IsDigit(C)) bHasDigit = true;
	}
	return bHasLetter && bHasDigit;
}

FString UGZAccountLoginManager::HashPassword(const FString& Password) const
{
	return FMD5::HashAnsiString(*Password);
}

FString UGZAccountLoginManager::GeneratePlayerID() const
{
	FGuid Guid = FGuid::NewGuid();
	return FString::Printf(TEXT("GZ_%s"), *Guid.ToString(EGuidFormats::Digits).Left(12));
}

FString UGZAccountLoginManager::GenerateVerificationCode() const
{
	int32 Code = FMath::RandRange(100000, 999999);
	return FString::Printf(TEXT("%06d"), Code);
}

// ===== Email Login =====
void UGZAccountLoginManager::LoginWithEmail(const FString& Email, const FString& Password)
{
	if (!ValidateEmail(Email))
	{
		OnLoginResult.Broadcast(false, TEXT("邮箱格式不正确"));
		return;
	}

	CurrentState = EGZLoginState::LoggingIn;
	CurrentChannel = EGZLoginChannel::Email;

	FGZPlayerAccount* Existing = FindAccountByEmail(Email);
	if (Existing)
	{
		FString HashedInput = HashPassword(Password);
		if (Existing->PasswordHash == HashedInput)
		{
			CurrentAccount = *Existing;
			CurrentAccount.LastLogin = FDateTime::Now();
			CurrentAccount.LoginCount++;
			CurrentState = EGZLoginState::LoggedIn;
			SaveAccountData();
			OnLoginResult.Broadcast(true, TEXT("登录成功"));
			return;
		}
	}

	CurrentState = EGZLoginState::LoginFailed;
	OnLoginResult.Broadcast(false, TEXT("邮箱或密码错误"));
}

void UGZAccountLoginManager::RegisterWithEmail(const FString& Email, const FString& Password, const FString& PlayerName)
{
	if (!ValidateEmail(Email))
	{
		OnRegisterResult.Broadcast(false, TEXT("邮箱格式不正确"));
		return;
	}
	if (!ValidatePassword(Password))
	{
		OnRegisterResult.Broadcast(false, TEXT("密码需要8-16位，包含字母和数字"));
		return;
	}
	if (CheckExistingAccount(Email, TEXT("")))
	{
		OnRegisterResult.Broadcast(false, TEXT("该邮箱已被注册"));
		return;
	}

	CurrentState = EGZLoginState::Registering;
	CurrentChannel = EGZLoginChannel::Email;

	FGZPlayerAccount NewAccount;
	NewAccount.PlayerID = GeneratePlayerID();
	NewAccount.PlayerName = PlayerName.IsEmpty() ? FString::Printf(TEXT("Player_%s"), *NewAccount.PlayerID.Right(6)) : PlayerName;
	NewAccount.Email = Email;
	NewAccount.PhoneNumber = TEXT("");
	NewAccount.PasswordHash = HashPassword(Password);
	NewAccount.bIsGuest = false;
	NewAccount.CreatedAt = FDateTime::Now();
	NewAccount.LastLogin = FDateTime::Now();
	NewAccount.LoginCount = 1;

	CurrentAccount = NewAccount;
	AddAccountToRegistry(NewAccount);
	CurrentState = EGZLoginState::LoggedIn;
	SaveAccountData();
	OnRegisterResult.Broadcast(true, TEXT("注册成功"));
}

void UGZAccountLoginManager::SendEmailVerificationCode(const FString& Email)
{
	if (IsVerificationLocked())
	{
		OnVerificationCodeSent.Broadcast(false);
		return;
	}
	if (!ValidateEmail(Email))
	{
		OnVerificationCodeSent.Broadcast(false);
		return;
	}

	CurrentVerificationCode = GenerateVerificationCode();
	CurrentVerificationTarget = Email;
	OnVerificationCodeSent.Broadcast(true);
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Email verification code sent to %s: %s"), *Email, *CurrentVerificationCode);
}

void UGZAccountLoginManager::VerifyEmailCode(const FString& Email, const FString& Code)
{
	if (Email == CurrentVerificationTarget && Code == CurrentVerificationCode)
	{
		ResetVerificationFailures();
		CurrentVerificationCode.Empty();
		CurrentVerificationTarget.Empty();
		OnLoginResult.Broadcast(true, TEXT("验证成功"));
	}
	else
	{
		HandleVerificationFailure();
		OnLoginResult.Broadcast(false, TEXT("验证码错误"));
	}
}

void UGZAccountLoginManager::HandleVerificationFailure()
{
	VerificationFailCount++;
	if (VerificationFailCount >= 5)
	{
		VerificationLockoutUntil = FDateTime::Now() + FTimespan::FromSeconds(30.0);
		VerificationFailCount = 0;
		UE_LOG(LogGuangzhouOpenWorld, Warning, TEXT("Verification locked for 30 seconds after 5 consecutive failures"));
	}
}

void UGZAccountLoginManager::ResetPasswordByEmail(const FString& Email)
{
	FGZPlayerAccount* Account = FindAccountByEmail(Email);
	if (Account)
	{
		SendEmailVerificationCode(Email);
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Password reset requested for: %s"), *Email);
	}
}

// ===== Phone Login =====
void UGZAccountLoginManager::LoginWithPhone(const FString& PhoneNumber, const FString& VerificationCode)
{
	if (!ValidatePhone(PhoneNumber))
	{
		OnLoginResult.Broadcast(false, TEXT("手机号格式不正确"));
		return;
	}

	CurrentState = EGZLoginState::LoggingIn;
	CurrentChannel = EGZLoginChannel::Phone;

	if (PhoneNumber == CurrentVerificationTarget && VerificationCode == CurrentVerificationCode)
	{
		ResetVerificationFailures();
		FGZPlayerAccount* Existing = FindAccountByPhone(PhoneNumber);
		if (Existing)
		{
			CurrentAccount = *Existing;
			CurrentAccount.LastLogin = FDateTime::Now();
			CurrentAccount.LoginCount++;
		}
		else
		{
			FGZPlayerAccount NewAccount;
			NewAccount.PlayerID = GeneratePlayerID();
			NewAccount.PlayerName = FString::Printf(TEXT("Player_%s"), *PhoneNumber.Right(6));
			NewAccount.PhoneNumber = PhoneNumber;
			NewAccount.PasswordHash = TEXT("");
			NewAccount.bIsGuest = false;
			NewAccount.CreatedAt = FDateTime::Now();
			NewAccount.LastLogin = FDateTime::Now();
			NewAccount.LoginCount = 1;
			CurrentAccount = NewAccount;
			AddAccountToRegistry(NewAccount);
		}
		CurrentVerificationCode.Empty();
		CurrentVerificationTarget.Empty();
		CurrentState = EGZLoginState::LoggedIn;
		SaveAccountData();
		OnLoginResult.Broadcast(true, TEXT("登录成功"));
	}
	else
	{
		HandleVerificationFailure();
		CurrentState = EGZLoginState::LoginFailed;
		OnLoginResult.Broadcast(false, TEXT("验证码错误"));
	}
}

void UGZAccountLoginManager::SendPhoneVerificationCode(const FString& PhoneNumber)
{
	if (IsVerificationLocked())
	{
		OnVerificationCodeSent.Broadcast(false);
		return;
	}
	if (!ValidatePhone(PhoneNumber))
	{
		OnVerificationCodeSent.Broadcast(false);
		return;
	}

	CurrentVerificationCode = GenerateVerificationCode();
	CurrentVerificationTarget = PhoneNumber;
	OnVerificationCodeSent.Broadcast(true);
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Phone verification code sent to %s: %s"), *PhoneNumber, *CurrentVerificationCode);
}

void UGZAccountLoginManager::VerifyPhoneCode(const FString& PhoneNumber, const FString& Code)
{
	if (PhoneNumber == CurrentVerificationTarget && Code == CurrentVerificationCode)
	{
		ResetVerificationFailures();
		CurrentVerificationCode.Empty();
		CurrentVerificationTarget.Empty();
		OnLoginResult.Broadcast(true, TEXT("验证成功"));
	}
	else
	{
		HandleVerificationFailure();
		OnLoginResult.Broadcast(false, TEXT("验证码错误"));
	}
}

void UGZAccountLoginManager::RecoverAccountByPhone(const FString& PhoneNumber)
{
	FGZPlayerAccount* Account = FindAccountByPhone(PhoneNumber);
	if (Account)
	{
		SendPhoneVerificationCode(PhoneNumber);
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Account recovery requested for: %s"), *PhoneNumber);
	}
}

// ===== Guest Login =====
void UGZAccountLoginManager::LoginAsGuest()
{
	CurrentState = EGZLoginState::LoggingIn;
	CurrentChannel = EGZLoginChannel::Guest;

	FGZPlayerAccount GuestAccount;
	GuestAccount.PlayerID = GeneratePlayerID();
	GuestAccount.PlayerName = FString::Printf(TEXT("Guest_%s"), *GuestAccount.PlayerID.Right(6));
	GuestAccount.bIsGuest = true;
	GuestAccount.GuestExpireTime = FDateTime::Now() + FTimespan::FromDays(7);
	GuestAccount.CreatedAt = FDateTime::Now();
	GuestAccount.LastLogin = FDateTime::Now();
	GuestAccount.LoginCount = 1;

	CurrentAccount = GuestAccount;
	CurrentState = EGZLoginState::LoggedIn;
	OnLoginResult.Broadcast(true, TEXT("游客登录成功 - 7天内绑定账号可保留数据"));
}

void UGZAccountLoginManager::BindGuestToEmail(const FString& Email, const FString& Password)
{
	if (!CurrentAccount.bIsGuest)
	{
		OnRegisterResult.Broadcast(false, TEXT("当前账号不是游客账号"));
		return;
	}
	if (!ValidateEmail(Email) || !ValidatePassword(Password))
	{
		OnRegisterResult.Broadcast(false, TEXT("邮箱或密码格式不正确"));
		return;
	}

	CurrentAccount.Email = Email;
	CurrentAccount.PasswordHash = HashPassword(Password);
	CurrentAccount.bIsGuest = false;
	AddAccountToRegistry(CurrentAccount);
	SaveAccountData();
	OnRegisterResult.Broadcast(true, TEXT("游客账号绑定邮箱成功"));
}

void UGZAccountLoginManager::BindGuestToPhone(const FString& PhoneNumber)
{
	if (!CurrentAccount.bIsGuest)
	{
		OnRegisterResult.Broadcast(false, TEXT("当前账号不是游客账号"));
		return;
	}
	if (!ValidatePhone(PhoneNumber))
	{
		OnRegisterResult.Broadcast(false, TEXT("手机号格式不正确"));
		return;
	}

	CurrentAccount.PhoneNumber = PhoneNumber;
	CurrentAccount.bIsGuest = false;
	AddAccountToRegistry(CurrentAccount);
	SaveAccountData();
	OnRegisterResult.Broadcast(true, TEXT("游客账号绑定手机号成功"));
}

// ===== Account Binding =====
void UGZAccountLoginManager::BindEmailToPhoneAccount(const FString& Email)
{
	if (CurrentAccount.PhoneNumber.IsEmpty())
	{
		OnRegisterResult.Broadcast(false, TEXT("当前账号不是手机号账号"));
		return;
	}
	if (!ValidateEmail(Email))
	{
		OnRegisterResult.Broadcast(false, TEXT("邮箱格式不正确"));
		return;
	}

	CurrentAccount.BoundEmail = Email;
	SaveAccountData();
	OnRegisterResult.Broadcast(true, TEXT("绑定邮箱成功，现在可以用邮箱登录"));
}

void UGZAccountLoginManager::BindPhoneToEmailAccount(const FString& PhoneNumber)
{
	if (CurrentAccount.Email.IsEmpty())
	{
		OnRegisterResult.Broadcast(false, TEXT("当前账号不是邮箱账号"));
		return;
	}
	if (!ValidatePhone(PhoneNumber))
	{
		OnRegisterResult.Broadcast(false, TEXT("手机号格式不正确"));
		return;
	}

	CurrentAccount.BoundPhone = PhoneNumber;
	SaveAccountData();
	OnRegisterResult.Broadcast(true, TEXT("绑定手机号成功，现在可以用手机号登录"));
}

bool UGZAccountLoginManager::IsAccountBound() const
{
	return !CurrentAccount.BoundEmail.IsEmpty() || !CurrentAccount.BoundPhone.IsEmpty();
}

// ===== General =====
void UGZAccountLoginManager::Logout()
{
	CurrentAccount = FGZPlayerAccount();
	CurrentState = EGZLoginState::LoggedOut;
	CurrentChannel = EGZLoginChannel::None;
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Player logged out"));
}

void UGZAccountLoginManager::SetAutoLogin(bool bEnabled)
{
	CurrentAccount.bAutoLogin = bEnabled;
	SaveAccountData();
}

void UGZAccountLoginManager::SetPlayerName(const FString& NewName)
{
	CurrentAccount.PlayerName = NewName;
	SaveAccountData();
}

// ===== Data Persistence =====
void UGZAccountLoginManager::SaveAccountData()
{
	TSharedPtr<FJsonObject> RootObj = MakeShareable(new FJsonObject);
	TArray<TSharedPtr<FJsonValue>> AccountArray;

	for (const FGZPlayerAccount& Acc : AccountRegistry)
	{
		TSharedPtr<FJsonObject> AccObj = MakeShareable(new FJsonObject);
		AccObj->SetStringField(TEXT("PlayerID"), Acc.PlayerID);
		AccObj->SetStringField(TEXT("PlayerName"), Acc.PlayerName);
		AccObj->SetStringField(TEXT("Email"), Acc.Email);
		AccObj->SetStringField(TEXT("PhoneNumber"), Acc.PhoneNumber);
		AccObj->SetStringField(TEXT("PasswordHash"), Acc.PasswordHash);
		AccObj->SetBoolField(TEXT("bIsGuest"), Acc.bIsGuest);
		AccObj->SetStringField(TEXT("GuestExpireTime"), Acc.GuestExpireTime.ToString());
		AccObj->SetStringField(TEXT("CreatedAt"), Acc.CreatedAt.ToString());
		AccObj->SetStringField(TEXT("LastLogin"), Acc.LastLogin.ToString());
		AccObj->SetNumberField(TEXT("LoginCount"), Acc.LoginCount);
		AccObj->SetNumberField(TEXT("TotalPlayTime"), Acc.TotalPlayTime);
		AccObj->SetNumberField(TEXT("PlayerLevel"), Acc.PlayerLevel);
		AccObj->SetNumberField(TEXT("Currency"), Acc.Currency);
		AccObj->SetBoolField(TEXT("bAutoLogin"), Acc.bAutoLogin);
		AccObj->SetBoolField(TEXT("bRememberAccount"), Acc.bRememberAccount);
		AccObj->SetStringField(TEXT("BoundEmail"), Acc.BoundEmail);
		AccObj->SetStringField(TEXT("BoundPhone"), Acc.BoundPhone);
		AccountArray.Add(MakeShareable(new FJsonValueObject(AccObj)));
	}

	RootObj->SetArrayField(TEXT("Accounts"), AccountArray);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(RootObj.ToSharedRef(), Writer);
	FFileHelper::SaveStringToFile(OutputString, *AccountsFilePath);
}

void UGZAccountLoginManager::LoadAccountData()
{
	AccountRegistry.Empty();

	FString JsonString;
	if (FFileHelper::LoadFileToString(JsonString, *AccountsFilePath))
	{
		TSharedPtr<FJsonObject> RootObj;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
		if (FJsonSerializer::Deserialize(Reader, RootObj) && RootObj.IsValid())
		{
			const TArray<TSharedPtr<FJsonValue>>* AccountArray;
			if (RootObj->TryGetArrayField(TEXT("Accounts"), AccountArray))
			{
				for (const TSharedPtr<FJsonValue>& Val : *AccountArray)
				{
					const TSharedPtr<FJsonObject>& Obj = Val->AsObject();
					FGZPlayerAccount Acc;
					Acc.PlayerID = Obj->GetStringField(TEXT("PlayerID"));
					Acc.PlayerName = Obj->GetStringField(TEXT("PlayerName"));
					Acc.Email = Obj->GetStringField(TEXT("Email"));
					Acc.PhoneNumber = Obj->GetStringField(TEXT("PhoneNumber"));
					Acc.PasswordHash = Obj->GetStringField(TEXT("PasswordHash"));
					Acc.bIsGuest = Obj->GetBoolField(TEXT("bIsGuest"));
					FDateTime::Parse(Obj->GetStringField(TEXT("GuestExpireTime")), Acc.GuestExpireTime);
					FDateTime::Parse(Obj->GetStringField(TEXT("CreatedAt")), Acc.CreatedAt);
					FDateTime::Parse(Obj->GetStringField(TEXT("LastLogin")), Acc.LastLogin);
					Acc.LoginCount = Obj->GetIntegerField(TEXT("LoginCount"));
					Acc.TotalPlayTime = Obj->GetNumberField(TEXT("TotalPlayTime"));
					Acc.PlayerLevel = Obj->GetIntegerField(TEXT("PlayerLevel"));
					Acc.Currency = Obj->GetIntegerField(TEXT("Currency"));
					Acc.bAutoLogin = Obj->GetBoolField(TEXT("bAutoLogin"));
					Acc.bRememberAccount = Obj->GetBoolField(TEXT("bRememberAccount"));
					Acc.BoundEmail = Obj->GetStringField(TEXT("BoundEmail"));
					Acc.BoundPhone = Obj->GetStringField(TEXT("BoundPhone"));
					AccountRegistry.Add(Acc);
				}
			}
		}
	}
}

bool UGZAccountLoginManager::CheckExistingAccount(const FString& Email, const FString& Phone) const
{
	for (const FGZPlayerAccount& Acc : AccountRegistry)
	{
		if (!Email.IsEmpty() && Acc.Email == Email) return true;
		if (!Phone.IsEmpty() && Acc.PhoneNumber == Phone) return true;
	}
	return false;
}

void UGZAccountLoginManager::AddAccountToRegistry(const FGZPlayerAccount& Account)
{
	AccountRegistry.Add(Account);
}

FGZPlayerAccount* UGZAccountLoginManager::FindAccountByEmail(const FString& Email)
{
	for (FGZPlayerAccount& Acc : AccountRegistry)
	{
		if (Acc.Email == Email || Acc.BoundEmail == Email) return &Acc;
	}
	return nullptr;
}

FGZPlayerAccount* UGZAccountLoginManager::FindAccountByPhone(const FString& Phone)
{
	for (FGZPlayerAccount& Acc : AccountRegistry)
	{
		if (Acc.PhoneNumber == Phone || Acc.BoundPhone == Phone) return &Acc;
	}
	return nullptr;
}

FGZPlayerAccount* UGZAccountLoginManager::FindAccountByID(const FString& PlayerID)
{
	for (FGZPlayerAccount& Acc : AccountRegistry)
	{
		if (Acc.PlayerID == PlayerID) return &Acc;
	}
	return nullptr;
}
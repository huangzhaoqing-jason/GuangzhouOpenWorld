#include "Game/GZLoginManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonWriter.h"
#include "Dom/JsonObject.h"

UGZLoginManager::UGZLoginManager()
{
	SaveFilePath = FPaths::ProjectSavedDir() / TEXT("Accounts.json");
	LoadAccountData();
}

void UGZLoginManager::LoginWithEmail(const FString& Email, const FString& Password)
{
	if (!ValidateEmail(Email))
	{
		CurrentState = EGZLoginState::LoginFailed;
		OnLoginResult.Broadcast(false, TEXT("Invalid email format"));
		return;
	}
	if (!ValidatePassword(Password))
	{
		CurrentState = EGZLoginState::LoginFailed;
		OnLoginResult.Broadcast(false, TEXT("Password must be 6-32 characters"));
		return;
	}
	CurrentState = EGZLoginState::LoggingIn;
	for (const FGZUserAccount& Acc : RegisteredAccounts)
	{
		if (Acc.Email.Equals(Email, ESearchCase::IgnoreCase))
		{
			if (Acc.PasswordHash.Equals(HashPassword(Password)))
			{
				CurrentAccount = Acc;
				CurrentAccount.LastLogin = FDateTime::Now();
				CurrentAccount.LoginCount++;
				CurrentState = EGZLoginState::LoggedIn;
				SaveAccountData();
				OnLoginResult.Broadcast(true, TEXT(""));
				return;
			}
			CurrentState = EGZLoginState::LoginFailed;
			OnLoginResult.Broadcast(false, TEXT("Incorrect password"));
			return;
		}
	}
	CurrentState = EGZLoginState::LoginFailed;
	OnLoginResult.Broadcast(false, TEXT("Account not found. Please register first."));
}

void UGZLoginManager::LoginWithPhone(const FString& PhoneNumber, const FString& Password)
{
	if (!ValidatePhone(PhoneNumber))
	{
		CurrentState = EGZLoginState::LoginFailed;
		OnLoginResult.Broadcast(false, TEXT("Invalid phone number format"));
		return;
	}
	if (!ValidatePassword(Password))
	{
		CurrentState = EGZLoginState::LoginFailed;
		OnLoginResult.Broadcast(false, TEXT("Password must be 6-32 characters"));
		return;
	}
	CurrentState = EGZLoginState::LoggingIn;
	for (const FGZUserAccount& Acc : RegisteredAccounts)
	{
		if (Acc.PhoneNumber.Equals(PhoneNumber))
		{
			if (Acc.PasswordHash.Equals(HashPassword(Password)))
			{
				CurrentAccount = Acc;
				CurrentAccount.LastLogin = FDateTime::Now();
				CurrentAccount.LoginCount++;
				CurrentState = EGZLoginState::LoggedIn;
				SaveAccountData();
				OnLoginResult.Broadcast(true, TEXT(""));
				return;
			}
			CurrentState = EGZLoginState::LoginFailed;
			OnLoginResult.Broadcast(false, TEXT("Incorrect password"));
			return;
		}
	}
	CurrentState = EGZLoginState::LoginFailed;
	OnLoginResult.Broadcast(false, TEXT("Account not found. Please register first."));
}

void UGZLoginManager::RegisterAccount(const FString& PlayerName, const FString& Email, const FString& PhoneNumber, const FString& Password)
{
	if (PlayerName.IsEmpty() || PlayerName.Len() < 2)
	{
		OnRegisterResult.Broadcast(false);
		return;
	}
	if (!ValidatePassword(Password))
	{
		OnRegisterResult.Broadcast(false);
		return;
	}
	if (CheckExistingAccount(Email, PhoneNumber))
	{
		OnRegisterResult.Broadcast(false);
		return;
	}
	FGZUserAccount NewAccount;
	NewAccount.PlayerName = PlayerName;
	NewAccount.Email = Email;
	NewAccount.PhoneNumber = PhoneNumber;
	NewAccount.PasswordHash = HashPassword(Password);
	NewAccount.CreatedAt = FDateTime::Now();
	NewAccount.LastLogin = FDateTime::Now();
	NewAccount.LoginCount = 1;
	RegisteredAccounts.Add(NewAccount);
	CurrentAccount = NewAccount;
	CurrentState = EGZLoginState::LoggedIn;
	SaveAccountData();
	OnRegisterResult.Broadcast(true);
}

void UGZLoginManager::Logout()
{
	CurrentState = EGZLoginState::LoggedOut;
	CurrentAccount = FGZUserAccount();
}

void UGZLoginManager::SetPlayerName(const FString& NewName)
{
	if (NewName.Len() >= 2)
	{
		CurrentAccount.PlayerName = NewName;
		SaveAccountData();
	}
}

bool UGZLoginManager::ValidateEmail(const FString& Email) const
{
	if (Email.IsEmpty()) return false;
	int32 AtIndex = -1;
	if (!Email.FindChar('@', AtIndex)) return false;
	if (AtIndex == 0 || AtIndex == Email.Len() - 1) return false;
	int32 DotIndex = -1;
	if (!Email.FindChar('.', DotIndex)) return false;
	if (DotIndex <= AtIndex + 1 || DotIndex == Email.Len() - 1) return false;
	return true;
}

bool UGZLoginManager::ValidatePhone(const FString& Phone) const
{
	if (Phone.IsEmpty()) return false;
	for (const TCHAR& Ch : Phone)
	{
		if (!FChar::IsDigit(Ch) && Ch != '+' && Ch != '-' && Ch != ' ')
		{
			return false;
		}
	}
	FString DigitsOnly;
	for (const TCHAR& Ch : Phone)
	{
		if (FChar::IsDigit(Ch)) DigitsOnly.AppendChar(Ch);
	}
	return DigitsOnly.Len() >= 7 && DigitsOnly.Len() <= 15;
}

bool UGZLoginManager::ValidatePassword(const FString& Password) const
{
	return Password.Len() >= 6 && Password.Len() <= 32;
}

FString UGZLoginManager::HashPassword(const FString& Password) const
{
	return FMD5::HashAnsiString(*Password);
}

void UGZLoginManager::SaveAccountData()
{
	TSharedPtr<FJsonObject> RootObj = MakeShareable(new FJsonObject);
	TArray<TSharedPtr<FJsonValue>> AccountsArray;
	for (const FGZUserAccount& Acc : RegisteredAccounts)
	{
		TSharedPtr<FJsonObject> AccObj = MakeShareable(new FJsonObject);
		AccObj->SetStringField(TEXT("PlayerName"), Acc.PlayerName);
		AccObj->SetStringField(TEXT("Email"), Acc.Email);
		AccObj->SetStringField(TEXT("PhoneNumber"), Acc.PhoneNumber);
		AccObj->SetStringField(TEXT("PasswordHash"), Acc.PasswordHash);
		AccObj->SetStringField(TEXT("CreatedAt"), Acc.CreatedAt.ToString());
		AccObj->SetStringField(TEXT("LastLogin"), Acc.LastLogin.ToString());
		AccObj->SetNumberField(TEXT("LoginCount"), Acc.LoginCount);
		AccObj->SetNumberField(TEXT("TotalPlayTime"), Acc.TotalPlayTime);
		AccObj->SetNumberField(TEXT("PlayerLevel"), Acc.PlayerLevel);
		AccObj->SetNumberField(TEXT("Currency"), Acc.Currency);
		AccountsArray.Add(MakeShareable(new FJsonValueObject(AccObj)));
	}
	RootObj->SetArrayField(TEXT("Accounts"), AccountsArray);
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(RootObj.ToSharedRef(), Writer);
	FFileHelper::SaveStringToFile(OutputString, *SaveFilePath);
}

void UGZLoginManager::LoadAccountData()
{
	FString JsonString;
	if (FFileHelper::LoadFileToString(JsonString, *SaveFilePath))
	{
		TSharedPtr<FJsonObject> RootObj;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
		if (FJsonSerializer::Deserialize(Reader, RootObj) && RootObj.IsValid())
		{
			const TArray<TSharedPtr<FJsonValue>>* AccountsArray;
			if (RootObj->TryGetArrayField(TEXT("Accounts"), AccountsArray))
			{
				for (const TSharedPtr<FJsonValue>& Val : *AccountsArray)
				{
					const TSharedPtr<FJsonObject>& Obj = Val->AsObject();
					FGZUserAccount Acc;
					Acc.PlayerName = Obj->GetStringField(TEXT("PlayerName"));
					Acc.Email = Obj->GetStringField(TEXT("Email"));
					Acc.PhoneNumber = Obj->GetStringField(TEXT("PhoneNumber"));
					Acc.PasswordHash = Obj->GetStringField(TEXT("PasswordHash"));
					FDateTime::Parse(Obj->GetStringField(TEXT("CreatedAt")), Acc.CreatedAt);
					FDateTime::Parse(Obj->GetStringField(TEXT("LastLogin")), Acc.LastLogin);
					Acc.LoginCount = Obj->GetIntegerField(TEXT("LoginCount"));
					Acc.TotalPlayTime = Obj->GetNumberField(TEXT("TotalPlayTime"));
					Acc.PlayerLevel = Obj->GetIntegerField(TEXT("PlayerLevel"));
					Acc.Currency = Obj->GetIntegerField(TEXT("Currency"));
					RegisteredAccounts.Add(Acc);
				}
			}
		}
	}
}

bool UGZLoginManager::CheckExistingAccount(const FString& Email, const FString& Phone) const
{
	for (const FGZUserAccount& Acc : RegisteredAccounts)
	{
		if (!Email.IsEmpty() && Acc.Email.Equals(Email, ESearchCase::IgnoreCase)) return true;
		if (!Phone.IsEmpty() && Acc.PhoneNumber.Equals(Phone)) return true;
	}
	return false;
}
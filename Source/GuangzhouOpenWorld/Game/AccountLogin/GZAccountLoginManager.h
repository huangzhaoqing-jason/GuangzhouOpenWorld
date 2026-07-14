#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Network/EOSSystem/GZEOSAuthInterface.h"
#include "GZAccountLoginManager.generated.h"

UENUM(BlueprintType)
enum class EGZLoginChannel : uint8
{
	None		UMETA(DisplayName = "None"),
	Email		UMETA(DisplayName = "Email"),
	Phone		UMETA(DisplayName = "Phone"),
	Guest		UMETA(DisplayName = "Guest"),
};

UENUM(BlueprintType)
enum class EGZLoginState : uint8
{
	LoggedOut		UMETA(DisplayName = "Logged Out"),
	LoggingIn		UMETA(DisplayName = "Logging In"),
	LoggedIn		UMETA(DisplayName = "Logged In"),
	LoginFailed		UMETA(DisplayName = "Login Failed"),
	Registering		UMETA(DisplayName = "Registering"),
	NeedVerification UMETA(DisplayName = "Need Verification"),
};

USTRUCT(BlueprintType)
struct FGZPlayerAccount
{
	GENERATED_BODY()

	UPROPERTY()
	FString PlayerID;

	UPROPERTY()
	FString PlayerName;

	UPROPERTY()
	FString Email;

	UPROPERTY()
	FString PhoneNumber;

	UPROPERTY()
	FString PasswordHash;

	UPROPERTY()
	bool bIsGuest = false;

	UPROPERTY()
	FDateTime GuestExpireTime;

	UPROPERTY()
	FDateTime CreatedAt;

	UPROPERTY()
	FDateTime LastLogin;

	UPROPERTY()
	int32 LoginCount = 0;

	UPROPERTY()
	float TotalPlayTime = 0.0f;

	UPROPERTY()
	int32 PlayerLevel = 1;

	UPROPERTY()
	int32 Currency = 0;

	UPROPERTY()
	bool bAutoLogin = false;

	UPROPERTY()
	bool bRememberAccount = false;

	UPROPERTY()
	FString BoundEmail;

	UPROPERTY()
	FString BoundPhone;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoginResult, bool, bSuccess, const FString&, ErrorMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRegisterResult, bool, bSuccess, const FString&, ErrorMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVerificationCodeSent, bool, bSuccess);

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZAccountLoginManager : public UObject
{
	GENERATED_BODY()

public:
	UGZAccountLoginManager();

	// ===== Email Login =====
	UFUNCTION(BlueprintCallable, Category = "Account|Email")
	void LoginWithEmail(const FString& Email, const FString& Password);

	UFUNCTION(BlueprintCallable, Category = "Account|Email")
	void RegisterWithEmail(const FString& Email, const FString& Password, const FString& PlayerName);

	UFUNCTION(BlueprintCallable, Category = "Account|Email")
	void SendEmailVerificationCode(const FString& Email);

	UFUNCTION(BlueprintCallable, Category = "Account|Email")
	void VerifyEmailCode(const FString& Email, const FString& Code);

	UFUNCTION(BlueprintCallable, Category = "Account|Email")
	void ResetPasswordByEmail(const FString& Email);

	// ===== Phone Login =====
	UFUNCTION(BlueprintCallable, Category = "Account|Phone")
	void LoginWithPhone(const FString& PhoneNumber, const FString& VerificationCode);

	UFUNCTION(BlueprintCallable, Category = "Account|Phone")
	void SendPhoneVerificationCode(const FString& PhoneNumber);

	UFUNCTION(BlueprintCallable, Category = "Account|Phone")
	void VerifyPhoneCode(const FString& PhoneNumber, const FString& Code);

	UFUNCTION(BlueprintCallable, Category = "Account|Phone")
	void RecoverAccountByPhone(const FString& PhoneNumber);

	// ===== Guest Login =====
	UFUNCTION(BlueprintCallable, Category = "Account|Guest")
	void LoginAsGuest();

	UFUNCTION(BlueprintCallable, Category = "Account|Guest")
	void BindGuestToEmail(const FString& Email, const FString& Password);

	UFUNCTION(BlueprintCallable, Category = "Account|Guest")
	void BindGuestToPhone(const FString& PhoneNumber);

	UFUNCTION(BlueprintPure, Category = "Account|Guest")
	bool CanGuestUseOnline() const { return false; }

	UFUNCTION(BlueprintPure, Category = "Account|Guest")
	bool CanGuestUseCloudSave() const { return false; }

	// ===== Account Binding =====
	UFUNCTION(BlueprintCallable, Category = "Account|Binding")
	void BindEmailToPhoneAccount(const FString& Email);

	UFUNCTION(BlueprintCallable, Category = "Account|Binding")
	void BindPhoneToEmailAccount(const FString& PhoneNumber);

	UFUNCTION(BlueprintCallable, Category = "Account|Binding")
	bool IsAccountBound() const;

	// ===== General =====
	UFUNCTION(BlueprintCallable, Category = "Account")
	void Logout();

	UFUNCTION(BlueprintCallable, Category = "Account")
	void SetAutoLogin(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Account")
	void SetRememberAccount(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Account")
	void TryAutoLogin();

	UFUNCTION(BlueprintPure, Category = "Account")
	bool IsLoggedIn() const { return CurrentState == EGZLoginState::LoggedIn; }

	UFUNCTION(BlueprintPure, Category = "Account")
	bool IsGuest() const { return CurrentAccount.bIsGuest; }

	UFUNCTION(BlueprintPure, Category = "Account")
	bool IsGuestExpired() const;

	UFUNCTION(BlueprintPure, Category = "Account")
	EGZLoginState GetLoginState() const { return CurrentState; }

	/** Returns the EOS auth interface stub. Real EOS SDK linkage required for cloud/encryption. */
	UFUNCTION(BlueprintPure, Category = "Account|EOS")
	UGZEOSAuthInterface* GetEOSAuthInterface() const { return EOSAuthInterface; }

	UFUNCTION(BlueprintPure, Category = "Account")
	EGZLoginChannel GetLoginChannel() const { return CurrentChannel; }

	UFUNCTION(BlueprintPure, Category = "Account")
	const FGZPlayerAccount& GetCurrentAccount() const { return CurrentAccount; }

	UFUNCTION(BlueprintPure, Category = "Account")
	FString GetPlayerID() const { return CurrentAccount.PlayerID; }

	UFUNCTION(BlueprintPure, Category = "Account")
	FString GetPlayerName() const { return CurrentAccount.PlayerName; }

	UFUNCTION(BlueprintCallable, Category = "Account")
	void SetPlayerName(const FString& NewName);

	UPROPERTY(BlueprintAssignable, Category = "Account")
	FOnLoginResult OnLoginResult;

	UPROPERTY(BlueprintAssignable, Category = "Account")
	FOnRegisterResult OnRegisterResult;

	UPROPERTY(BlueprintAssignable, Category = "Account")
	FOnVerificationCodeSent OnVerificationCodeSent;

	// Verification code timers
	UPROPERTY(BlueprintReadOnly, Category = "Account")
	float PhoneCodeCooldown = 10.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Account")
	float PhoneCodeExpiry = 60.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Account")
	float EmailCodeExpiry = 300.0f;

	// Verification lockout: 5 consecutive failures lock code sending for 30 seconds
	UPROPERTY(BlueprintReadOnly, Category = "Account")
	int32 VerificationFailCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Account")
	FDateTime VerificationLockoutUntil;

	UFUNCTION(BlueprintPure, Category = "Account")
	bool IsVerificationLocked() const { return FDateTime::Now() < VerificationLockoutUntil; }

private:
	bool ValidateEmail(const FString& Email) const;
	bool ValidatePhone(const FString& Phone) const;
	bool ValidatePassword(const FString& Password) const;
	FString HashPassword(const FString& Password) const;
	FString GeneratePlayerID() const;
	FString GenerateVerificationCode() const;
	void SaveAccountData();
	void LoadAccountData();
	bool CheckExistingAccount(const FString& Email, const FString& Phone) const;
	void AddAccountToRegistry(const FGZPlayerAccount& Account);
	FGZPlayerAccount* FindAccountByEmail(const FString& Email);
	FGZPlayerAccount* FindAccountByPhone(const FString& Phone);
	FGZPlayerAccount* FindAccountByID(const FString& PlayerID);
	void HandleVerificationFailure();
	void ResetVerificationFailures() { VerificationFailCount = 0; VerificationLockoutUntil = FDateTime::MinValue(); }
	bool CanSendVerificationCode() const { return !IsVerificationLocked(); }

	FString AccountsFilePath;
	FString CurrentVerificationCode;
	FString CurrentVerificationTarget;
	FDateTime CurrentVerificationCodeSentTime;
	FDateTime CurrentVerificationCodeExpiryTime;
	FDateTime PhoneCodeLastSentTime;
	EGZLoginChannel CurrentChannel = EGZLoginChannel::None;
	EGZLoginState CurrentState = EGZLoginState::LoggedOut;
	FGZPlayerAccount CurrentAccount;
	TArray<FGZPlayerAccount> AccountRegistry;
	FTimerHandle PhoneCodeTimerHandle;
	FTimerHandle EmailCodeTimerHandle;

	UPROPERTY()
	UGZEOSAuthInterface* EOSAuthInterface = nullptr;
	float PhoneCodeRemaining = 0.0f;
	float EmailCodeRemaining = 0.0f;

	bool IsVerificationCodeValid(const FString& Code) const;
	bool IsPhoneCodeInCooldown() const;
};
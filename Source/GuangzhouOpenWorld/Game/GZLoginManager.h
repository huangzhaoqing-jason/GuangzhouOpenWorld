#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZLoginManager.generated.h"

UENUM(BlueprintType)
enum class EGZLoginMethod : uint8
{
	Email		UMETA(DisplayName = "Email Login"),
	Phone		UMETA(DisplayName = "Phone Login"),
};

UENUM(BlueprintType)
enum class EGZLoginState : uint8
{
	LoggedOut	UMETA(DisplayName = "Logged Out"),
	LoggingIn	UMETA(DisplayName = "Logging In"),
	LoggedIn	UMETA(DisplayName = "Logged In"),
	LoginFailed	UMETA(DisplayName = "Login Failed"),
};

USTRUCT(BlueprintType)
struct FGZUserAccount
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Email;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PhoneNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PasswordHash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDateTime CreatedAt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDateTime LastLogin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LoginCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TotalPlayTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PlayerLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Currency = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoginResult, bool, bSuccess, const FString&, ErrorMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRegisterResult, bool, bSuccess);

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZLoginManager : public UObject
{
	GENERATED_BODY()

public:
	UGZLoginManager();

	UFUNCTION(BlueprintCallable, Category = "Login")
	void LoginWithEmail(const FString& Email, const FString& Password);

	UFUNCTION(BlueprintCallable, Category = "Login")
	void LoginWithPhone(const FString& PhoneNumber, const FString& Password);

	UFUNCTION(BlueprintCallable, Category = "Login")
	void RegisterAccount(const FString& PlayerName, const FString& Email, const FString& PhoneNumber, const FString& Password);

	UFUNCTION(BlueprintCallable, Category = "Login")
	void Logout();

	UFUNCTION(BlueprintPure, Category = "Login")
	EGZLoginState GetLoginState() const { return CurrentState; }

	UFUNCTION(BlueprintPure, Category = "Login")
	const FGZUserAccount& GetCurrentAccount() const { return CurrentAccount; }

	UFUNCTION(BlueprintPure, Category = "Login")
	bool IsLoggedIn() const { return CurrentState == EGZLoginState::LoggedIn; }

	UFUNCTION(BlueprintCallable, Category = "Login")
	void SetPlayerName(const FString& NewName);

	UPROPERTY(BlueprintAssignable, Category = "Login")
	FOnLoginResult OnLoginResult;

	UPROPERTY(BlueprintAssignable, Category = "Login")
	FOnRegisterResult OnRegisterResult;

private:
	bool ValidateEmail(const FString& Email) const;
	bool ValidatePhone(const FString& Phone) const;
	bool ValidatePassword(const FString& Password) const;
	FString HashPassword(const FString& Password) const;
	void SaveAccountData();
	void LoadAccountData();
	bool CheckExistingAccount(const FString& Email, const FString& Phone) const;

	UPROPERTY()
	EGZLoginState CurrentState = EGZLoginState::LoggedOut;

	UPROPERTY()
	FGZUserAccount CurrentAccount;

	UPROPERTY()
	TArray<FGZUserAccount> RegisteredAccounts;

	FString SaveFilePath;
};
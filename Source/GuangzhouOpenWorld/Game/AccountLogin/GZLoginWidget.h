#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Game/AccountLogin/GZAccountLoginManager.h"
#include "GZLoginWidget.generated.h"

UCLASS()
class GUANGZHOUOPENWORLD_API UGZLoginWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UGZLoginWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Login")
	void SwitchToEmailLogin();

	UFUNCTION(BlueprintCallable, Category = "Login")
	void SwitchToPhoneLogin();

	UFUNCTION(BlueprintCallable, Category = "Login")
	void SwitchToRegister();

	UFUNCTION(BlueprintCallable, Category = "Login")
	void SubmitEmailLogin();

	UFUNCTION(BlueprintCallable, Category = "Login")
	void SubmitPhoneLogin();

	UFUNCTION(BlueprintCallable, Category = "Login")
	void SubmitEmailRegister();

	UFUNCTION(BlueprintCallable, Category = "Login")
	void RequestEmailCode();

	UFUNCTION(BlueprintCallable, Category = "Login")
	void RequestPhoneCode();

	UFUNCTION(BlueprintCallable, Category = "Login")
	void LoginAsGuest();

	UFUNCTION(BlueprintPure, Category = "Login")
	bool IsPhoneCodeCooldown() const;

	UFUNCTION(BlueprintPure, Category = "Login")
	float GetPhoneCodeCooldownPercent() const;

	UFUNCTION(BlueprintPure, Category = "Login")
	FString GetPhoneCodeCooldownText() const;

	UFUNCTION(BlueprintCallable, Category = "Login")
	void SetRememberAccount(bool bEnabled);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Login")
	EGZLoginChannel SelectedChannel = EGZLoginChannel::Email;

	UPROPERTY(BlueprintReadOnly, Category = "Login")
	FString InputEmail;

	UPROPERTY(BlueprintReadOnly, Category = "Login")
	FString InputPassword;

	UPROPERTY(BlueprintReadOnly, Category = "Login")
	FString InputPhone;

	UPROPERTY(BlueprintReadOnly, Category = "Login")
	FString InputVerificationCode;

	UPROPERTY(BlueprintReadOnly, Category = "Login")
	FString InputPlayerName;

	UPROPERTY(BlueprintReadOnly, Category = "Login")
	bool bRememberAccount = true;

	UPROPERTY(BlueprintReadOnly, Category = "Login")
	float PhoneCodeCooldownRemaining = 0.0f;

	FTimerHandle CooldownTimer;

	void OnLoginResult(bool bSuccess, const FString& Message);
	void OnRegisterResult(bool bSuccess, const FString& Message);
	void OnVerificationSent(bool bSuccess);
	void UpdateCooldown();
};
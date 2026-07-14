#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZEOSAuthInterface.generated.h"

/**
 * v5.3 EOS SDK 1.19.0.7 authentication interface stub.
 *
 * IMPORTANT: This class declares the expected integration surface for the EOS
 * Auth interface. Actual EOS SDK headers (eos_auth.h, eos_sdk.h, etc.) are
 * required to implement the function bodies. The cpp file currently contains
 * placeholder logic that logs the intended EOS call so the project can compile
 * and run without the EOS binary SDK present in this sandbox.
 */
UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZEOSAuthInterface : public UObject
{
	GENERATED_BODY()

public:
	UGZEOSAuthInterface();

	/** Initialize the EOS SDK Auth interface. Returns false if SDK is unavailable. */
	UFUNCTION(BlueprintCallable, Category = "EOS|Auth")
	bool Initialize();

	/** Query whether the EOS SDK is initialized and ready. */
	UFUNCTION(BlueprintPure, Category = "EOS|Auth")
	bool IsInitialized() const { return bInitialized; }

	/** Login with email + password via EOS Connect/Auth. */
	UFUNCTION(BlueprintCallable, Category = "EOS|Auth")
	void LoginWithEmail(const FString& Email, const FString& PasswordHash);

	/** Login with phone + verification code via EOS Connect/Auth. */
	UFUNCTION(BlueprintCallable, Category = "EOS|Auth")
	void LoginWithPhone(const FString& PhoneNumber, const FString& VerificationCode);

	/** Request password reset through EOS backend. */
	UFUNCTION(BlueprintCallable, Category = "EOS|Auth")
	void RequestPasswordReset(const FString& Email);

	/** Bind a phone number to an existing EOS account. */
	UFUNCTION(BlueprintCallable, Category = "EOS|Auth")
	void BindPhoneToAccount(const FString& PlayerID, const FString& PhoneNumber);

	/** Bind an email to an existing EOS account. */
	UFUNCTION(BlueprintCallable, Category = "EOS|Auth")
	void BindEmailToAccount(const FString& PlayerID, const FString& Email);

	/** Pull cloud save/settings for the logged-in player. */
	UFUNCTION(BlueprintCallable, Category = "EOS|Cloud")
	void PullCloudPlayerData(const FString& PlayerID);

	/** Push local save/settings to EOS cloud. */
	UFUNCTION(BlueprintCallable, Category = "EOS|Cloud")
	void PushCloudPlayerData(const FString& PlayerID, const FString& JsonPayload);

	/** Encrypt account payload using EOS latest encryption protocol. */
	UFUNCTION(BlueprintCallable, Category = "EOS|Security")
	FString EncryptAccountData(const FString& PlainPayload) const;

	/** Decrypt account payload using EOS latest encryption protocol. */
	UFUNCTION(BlueprintCallable, Category = "EOS|Security")
	FString DecryptAccountData(const FString& EncryptedPayload) const;

private:
	bool bInitialized = false;
};

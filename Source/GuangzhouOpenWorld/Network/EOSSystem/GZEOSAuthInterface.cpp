#include "Network/EOSSystem/GZEOSAuthInterface.h"
#include "GuangzhouOpenWorld.h"

UGZEOSAuthInterface::UGZEOSAuthInterface()
{
	bInitialized = false;
}

bool UGZEOSAuthInterface::Initialize()
{
	// NOTE: Real implementation requires:
	//   #include "eos_sdk.h"
	//   #include "eos_auth.h"
	//   EOS_Initialize(...)
	//   EOS_Platform_Create(...)
	//   EOS_Auth_GetAuthInterface(...)
	//
	// In this sandbox the EOS SDK binary is not available, so we leave the
	// interface stubbed and log the intended initialization.
	bInitialized = false;
	UE_LOG(LogGuangzhouOpenWorld, Warning, TEXT("EOS Auth Interface: Initialize() called but EOS SDK 1.19.0.7 is not linked in this environment."));
	return false;
}

void UGZEOSAuthInterface::LoginWithEmail(const FString& Email, const FString& PasswordHash)
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("EOS Auth Interface: LoginWithEmail(%s) [stub - requires EOS SDK]"), *Email);
}

void UGZEOSAuthInterface::LoginWithPhone(const FString& PhoneNumber, const FString& VerificationCode)
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("EOS Auth Interface: LoginWithPhone(%s) [stub - requires EOS SDK]"), *PhoneNumber);
}

void UGZEOSAuthInterface::RequestPasswordReset(const FString& Email)
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("EOS Auth Interface: RequestPasswordReset(%s) [stub - requires EOS SDK]"), *Email);
}

void UGZEOSAuthInterface::BindPhoneToAccount(const FString& PlayerID, const FString& PhoneNumber)
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("EOS Auth Interface: BindPhoneToAccount(%s, %s) [stub - requires EOS SDK]"), *PlayerID, *PhoneNumber);
}

void UGZEOSAuthInterface::BindEmailToAccount(const FString& PlayerID, const FString& Email)
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("EOS Auth Interface: BindEmailToAccount(%s, %s) [stub - requires EOS SDK]"), *PlayerID, *Email);
}

void UGZEOSAuthInterface::PullCloudPlayerData(const FString& PlayerID)
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("EOS Auth Interface: PullCloudPlayerData(%s) [stub - requires EOS SDK]"), *PlayerID);
}

void UGZEOSAuthInterface::PushCloudPlayerData(const FString& PlayerID, const FString& JsonPayload)
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("EOS Auth Interface: PushCloudPlayerData(%s, len=%d) [stub - requires EOS SDK]"), *PlayerID, JsonPayload.Len());
}

FString UGZEOSAuthInterface::EncryptAccountData(const FString& PlainPayload) const
{
	// Placeholder: return base64-like encoded string. Replace with EOS_Encrypt/
	// EOS_DataEncryptionKey APIs once SDK is linked.
	UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("EOS Auth Interface: EncryptAccountData(len=%d) [stub]"), PlainPayload.Len());
	return PlainPayload;
}

FString UGZEOSAuthInterface::DecryptAccountData(const FString& EncryptedPayload) const
{
	UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("EOS Auth Interface: DecryptAccountData(len=%d) [stub]"), EncryptedPayload.Len());
	return EncryptedPayload;
}

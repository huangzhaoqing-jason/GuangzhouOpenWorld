#include "Game/AccountLogin/GZLoginWidget.h"
#include "Game/GZGameInstance.h"
#include "GuangzhouOpenWorld.h"
#include "TimerManager.h"

UGZLoginWidget::UGZLoginWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGZLoginWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SelectedChannel = EGZLoginChannel::Email;
}

void UGZLoginWidget::SwitchToEmailLogin()
{
	SelectedChannel = EGZLoginChannel::Email;
}

void UGZLoginWidget::SwitchToPhoneLogin()
{
	SelectedChannel = EGZLoginChannel::Phone;
}

void UGZLoginWidget::SwitchToRegister()
{
}

void UGZLoginWidget::SubmitEmailLogin()
{
	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	if (!GI) return;

	UGZAccountLoginManager* LoginMgr = GI->GetLoginManager();
	if (!LoginMgr) return;

	LoginMgr->OnLoginResult.AddUObject(this, &UGZLoginWidget::OnLoginResult);
	LoginMgr->LoginWithEmail(InputEmail, InputPassword);
}

void UGZLoginWidget::SubmitPhoneLogin()
{
	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	if (!GI) return;

	UGZAccountLoginManager* LoginMgr = GI->GetLoginManager();
	if (!LoginMgr) return;

	LoginMgr->OnLoginResult.AddUObject(this, &UGZLoginWidget::OnLoginResult);
	LoginMgr->LoginWithPhone(InputPhone, InputVerificationCode);
}

void UGZLoginWidget::SubmitEmailRegister()
{
	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	if (!GI) return;

	UGZAccountLoginManager* LoginMgr = GI->GetLoginManager();
	if (!LoginMgr) return;

	LoginMgr->OnRegisterResult.AddUObject(this, &UGZLoginWidget::OnRegisterResult);
	LoginMgr->RegisterWithEmail(InputEmail, InputPassword, InputPlayerName);
}

void UGZLoginWidget::RequestEmailCode()
{
	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	if (!GI) return;

	UGZAccountLoginManager* LoginMgr = GI->GetLoginManager();
	if (!LoginMgr) return;

	LoginMgr->OnVerificationCodeSent.AddUObject(this, &UGZLoginWidget::OnVerificationSent);
	LoginMgr->SendEmailVerificationCode(InputEmail);
}

void UGZLoginWidget::RequestPhoneCode()
{
	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	if (!GI) return;

	UGZAccountLoginManager* LoginMgr = GI->GetLoginManager();
	if (!LoginMgr) return;

	if (PhoneCodeCooldownRemaining > 0.0f) return;

	LoginMgr->OnVerificationCodeSent.AddUObject(this, &UGZLoginWidget::OnVerificationSent);
	LoginMgr->SendPhoneVerificationCode(InputPhone);

	PhoneCodeCooldownRemaining = 10.0f;
	GetWorld()->GetTimerManager().SetTimer(CooldownTimer, this, &UGZLoginWidget::UpdateCooldown, 1.0f, true);
}

void UGZLoginWidget::LoginAsGuest()
{
	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	if (!GI) return;

	UGZAccountLoginManager* LoginMgr = GI->GetLoginManager();
	if (!LoginMgr) return;

	LoginMgr->OnLoginResult.AddUObject(this, &UGZLoginWidget::OnLoginResult);
	LoginMgr->LoginAsGuest();
}

bool UGZLoginWidget::IsPhoneCodeCooldown() const
{
	return PhoneCodeCooldownRemaining > 0.0f;
}

float UGZLoginWidget::GetPhoneCodeCooldownPercent() const
{
	if (PhoneCodeCooldownRemaining <= 0.0f) return 0.0f;
	return PhoneCodeCooldownRemaining / 10.0f;
}

FString UGZLoginWidget::GetPhoneCodeCooldownText() const
{
	int32 Seconds = FMath::CeilToInt(PhoneCodeCooldownRemaining);
	return FString::Printf(TEXT("重新获取 (%d)"), Seconds);
}

void UGZLoginWidget::SetRememberAccount(bool bEnabled)
{
	bRememberAccount = bEnabled;
}

void UGZLoginWidget::OnLoginResult(bool bSuccess, const FString& Message)
{
	if (bSuccess)
	{
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Login successful: %s"), *Message);
		UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
		if (GI)
		{
			GI->SetGameState(EGZGameState::MainMenu);
		}
	}
	else
	{
		UE_LOG(LogGuangzhouOpenWorld, Warning, TEXT("Login failed: %s"), *Message);
	}
}

void UGZLoginWidget::OnRegisterResult(bool bSuccess, const FString& Message)
{
	if (bSuccess)
	{
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Registration successful: %s"), *Message);
	}
	else
	{
		UE_LOG(LogGuangzhouOpenWorld, Warning, TEXT("Registration failed: %s"), *Message);
	}
}

void UGZLoginWidget::OnVerificationSent(bool bSuccess)
{
	if (bSuccess)
	{
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Verification code sent"));
	}
}

void UGZLoginWidget::UpdateCooldown()
{
	if (PhoneCodeCooldownRemaining > 0.0f)
	{
		PhoneCodeCooldownRemaining -= 1.0f;
		if (PhoneCodeCooldownRemaining <= 0.0f)
		{
			PhoneCodeCooldownRemaining = 0.0f;
			GetWorld()->GetTimerManager().ClearTimer(CooldownTimer);
		}
	}
}
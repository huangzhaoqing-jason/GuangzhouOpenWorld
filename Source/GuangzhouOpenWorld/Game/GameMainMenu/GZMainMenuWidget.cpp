#include "Game/GameMainMenu/GZMainMenuWidget.h"
#include "Game/GZGameInstance.h"
#include "Game/GZSaveGameManager.h"
#include "Game/AccountLogin/GZAccountLoginManager.h"
#include "GuangzhouOpenWorld.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

UGZMainMenuWidget::UGZMainMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGZMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	RefreshUI();
}

void UGZMainMenuWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UGZMainMenuWidget::RefreshUI()
{
	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	if (!GI) return;

	UGZAccountLoginManager* LoginMgr = GI->GetLoginManager();
	if (LoginMgr && LoginMgr->IsLoggedIn())
	{
		PlayerName = LoginMgr->GetPlayerName();
		PlayerLevel = LoginMgr->GetCurrentAccount().PlayerLevel;
		Currency = LoginMgr->GetCurrentAccount().Currency;
	}

	UGZSaveGameManager* SaveMgr = GI->GetSaveManager();
	if (SaveMgr)
	{
		bHasContinueSave = SaveMgr->DoesSaveExist(0);
	}
}

void UGZMainMenuWidget::SetActiveTab(EGZMainMenuTab Tab)
{
	ActiveTab = Tab;
}

void UGZMainMenuWidget::OnContinueGame()
{
	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	if (!GI) return;

	UGZSaveGameManager* SaveMgr = GI->GetSaveManager();
	if (SaveMgr && SaveMgr->DoesSaveExist(0))
	{
		SaveMgr->LoadGame(0);
		GI->LoadGameMap(TEXT("GuangzhouCBD"));
	}
}

void UGZMainMenuWidget::OnNewGame()
{
	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->SetGameState(EGZGameState::CharacterSelect);
	}
}

void UGZMainMenuWidget::OnLoadGame(int32 SlotIndex)
{
	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	if (!GI) return;

	UGZSaveGameManager* SaveMgr = GI->GetSaveManager();
	if (SaveMgr && SaveMgr->DoesSaveExist(SlotIndex))
	{
		SaveMgr->LoadGame(SlotIndex);
		GI->LoadGameMap(TEXT("GuangzhouCBD"));
	}
}

void UGZMainMenuWidget::OnMultiplayer()
{
	SetActiveTab(EGZMainMenuTab::Multiplayer);
}

void UGZMainMenuWidget::OnCreatePrivateRoom()
{
	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	if (GI)
	{
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Creating private multiplayer room via EOS..."));
		GI->LoadGameMap(TEXT("GuangzhouMultiplayer"));
	}
}

void UGZMainMenuWidget::OnJoinPublicMatch()
{
	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	if (GI)
	{
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Joining public match via EOS matchmaking..."));
		GI->LoadGameMap(TEXT("GuangzhouMultiplayer"));
	}
}

void UGZMainMenuWidget::OnInviteFriend(const FString& PlayerID)
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Inviting friend: %s"), *PlayerID);
}

void UGZMainMenuWidget::OnOpenSettings()
{
	SetActiveTab(EGZMainMenuTab::Settings);
}

void UGZMainMenuWidget::OnQuitGame()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true);
}

void UGZMainMenuWidget::OnCharacterSelect()
{
	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->SetGameState(EGZGameState::CharacterSelect);
	}
}

bool UGZMainMenuWidget::HasSaveData() const
{
	return bHasContinueSave;
}

TArray<FString> UGZMainMenuWidget::GetSaveSlotDisplayNames() const
{
	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	if (GI && GI->GetSaveManager())
	{
		return GI->GetSaveManager()->GetSaveSlotNames();
	}
	return TArray<FString>();
}

FString UGZMainMenuWidget::GetPlayerDisplayName() const
{
	return PlayerName;
}
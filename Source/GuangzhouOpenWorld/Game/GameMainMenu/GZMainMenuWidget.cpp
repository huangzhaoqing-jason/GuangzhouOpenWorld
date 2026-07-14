#include "Game/GameMainMenu/GZMainMenuWidget.h"
#include "Game/GameMainMenu/GZCharacterSelectWidget.h"
#include "Game/SystemSettings/GZSettingsPanelWidget.h"
#include "Game/GZGameInstance.h"
#include "Game/GZSaveGameManager.h"
#include "Game/AccountLogin/GZAccountLoginManager.h"
#include "GuangzhouOpenWorld.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/Guid.h"

UGZMainMenuWidget::UGZMainMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGZMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->OnGameStateChanged.AddDynamic(this, &UGZMainMenuWidget::HandleGameStateChanged);
	}

	RefreshUI();
}

void UGZMainMenuWidget::NativeDestruct()
{
	OnCloseSubWidget();

	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->OnGameStateChanged.RemoveDynamic(this, &UGZMainMenuWidget::HandleGameStateChanged);
	}

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
	if (PlayerID.IsEmpty())
	{
		UE_LOG(LogGuangzhouOpenWorld, Warning, TEXT("Cannot invite: empty PlayerID"));
		return;
	}

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Inviting friend %s to room %s (max %d players)"),
		*PlayerID, *CurrentRoomID, MaxRoomPlayers);
}

void UGZMainMenuWidget::OnOpenSaveManagement()
{
	SetActiveTab(EGZMainMenuTab::SaveManagement);
}

void UGZMainMenuWidget::OnDownloadCloudSave()
{
	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	if (!GI) return;

	UGZSaveGameManager* SaveMgr = GI->GetSaveManager();
	if (SaveMgr)
	{
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Downloading cloud save from EOS with retry..."));
		SaveMgr->RetryCloudLoad(0);
	}
}

void UGZMainMenuWidget::OnBackupSave()
{
	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	if (!GI) return;

	UGZSaveGameManager* SaveMgr = GI->GetSaveManager();
	if (SaveMgr)
	{
		SaveMgr->SaveGame(0, TEXT("ManualBackup"));
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Manual save backup created"));
	}
}

void UGZMainMenuWidget::OnRestoreSave(int32 SaveIndex)
{
	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	if (!GI) return;

	UGZSaveGameManager* SaveMgr = GI->GetSaveManager();
	if (SaveMgr && SaveMgr->DoesSaveExist(SaveIndex))
	{
		SaveMgr->LoadGame(SaveIndex);
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Restored save slot %d"), SaveIndex);
	}
}

void UGZMainMenuWidget::OnOpenSettings()
{
	SetActiveTab(EGZMainMenuTab::Settings);
	OnOpenSettingsPanel();
}

void UGZMainMenuWidget::OnQuitGame()
{
	bQuitConfirmationVisible = true;
}

void UGZMainMenuWidget::OnConfirmQuit()
{
	bQuitConfirmationVisible = false;
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true);
}

void UGZMainMenuWidget::OnCancelQuit()
{
	bQuitConfirmationVisible = false;
}

void UGZMainMenuWidget::OnHostRoom(int32 MaxPlayers, bool bPrivate)
{
	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	if (!GI) return;

	int32 FinalMaxPlayers = FMath::Clamp(MaxPlayers, 2, MaxRoomPlayers);
	CurrentRoomID = FString::Printf(TEXT("GZ_%s"), *FGuid::NewGuid().ToString(EGuidFormats::Digits).Left(8));

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Hosting %s room %s with max %d players"),
		bPrivate ? TEXT("private") : TEXT("public"), *CurrentRoomID, FinalMaxPlayers);

	GI->LoadGameMap(TEXT("GuangzhouMultiplayer"));
}

void UGZMainMenuWidget::OnRequestJoinRoomByID(const FString& RoomID)
{
	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	if (!GI) return;

	if (RoomID.IsEmpty())
	{
		UE_LOG(LogGuangzhouOpenWorld, Warning, TEXT("Cannot join room: empty RoomID"));
		return;
	}

	CurrentRoomID = RoomID;
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Joining room by ID: %s"), *RoomID);
	GI->LoadGameMap(TEXT("GuangzhouMultiplayer"));
}

void UGZMainMenuWidget::OnCharacterSelect()
{
	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->SetGameState(EGZGameState::CharacterSelect);
	}

	OnCloseSubWidget();

	if (!CharacterSelectWidgetClass) return;

	UGZCharacterSelectWidget* Widget = CreateWidget<UGZCharacterSelectWidget>(GetWorld(), CharacterSelectWidgetClass);
	if (Widget)
	{
		Widget->OnCharacterSelected.AddDynamic(this, &UGZMainMenuWidget::OnCharacterSelected);
		Widget->AddToViewport(10);
		ActiveSubWidget = Widget;
	}
}

void UGZMainMenuWidget::OnOpenSettingsPanel()
{
	SetActiveTab(EGZMainMenuTab::Settings);
	OnCloseSubWidget();

	if (!SettingsPanelWidgetClass) return;

	UGZSettingsPanelWidget* Widget = CreateWidget<UGZSettingsPanelWidget>(GetWorld(), SettingsPanelWidgetClass);
	if (Widget)
	{
		Widget->AddToViewport(10);
		ActiveSubWidget = Widget;
	}
}

void UGZMainMenuWidget::OnCloseSubWidget()
{
	if (ActiveSubWidget)
	{
		ActiveSubWidget->RemoveFromParent();
		ActiveSubWidget = nullptr;
	}
}

void UGZMainMenuWidget::OnSettingsPanelClosed()
{
	OnCloseSubWidget();
}

void UGZMainMenuWidget::OnCharacterSelected(int32 SelectedIndex)
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("MainMenu received character selection index %d"), SelectedIndex);
}

void UGZMainMenuWidget::HandleGameStateChanged()
{
	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	if (!GI) return;

	if (GI->GetGameState() == EGZGameState::MainMenu)
	{
		OnCloseSubWidget();
		RefreshUI();
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
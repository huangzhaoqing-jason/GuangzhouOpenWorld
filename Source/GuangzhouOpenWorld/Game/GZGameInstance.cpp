#include "Game/GZGameInstance.h"
#include "Game/AccountLogin/GZAccountLoginManager.h"
#include "Game/GZSaveGameManager.h"
#include "Game/GZSettingsManager.h"
#include "Game/SystemSettings/GZSystemSettings.h"
#include "Game/GZMissionManager.h"
#include "Game/GZInventoryManager.h"
#include "GuangzhouOpenWorld.h"
#include "Kismet/GameplayStatics.h"

UGZGameInstance::UGZGameInstance()
{
	CurrentGameState = EGZGameState::None;
}

void UGZGameInstance::Init()
{
	Super::Init();

	AccountLoginManager = NewObject<UGZAccountLoginManager>(this);
	SaveManager = NewObject<UGZSaveGameManager>(this);
	SettingsManager = NewObject<UGZSettingsManager>(this);
	SystemSettings = NewObject<UGZSystemSettings>(this);
	MissionManager = NewObject<UGZMissionManager>(this);
	InventoryManager = NewObject<UGZInventoryManager>(this);

	SettingsManager->LoadSettings();
	SystemSettings->LoadAllSettings();

	if (MissionManager)
	{
		const int32 Loaded = MissionManager->LoadAct1MissionsFromJson();
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("GZGameInstance: Act1 missions loaded = %d"), Loaded);
	}

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("GZGameInstance initialized - all 6 managers ready"));
}

void UGZGameInstance::Shutdown()
{
	if (SettingsManager) SettingsManager->SaveSettings();
	if (SystemSettings) SystemSettings->SaveAllSettings();

	Super::Shutdown();
}

void UGZGameInstance::SetGameState(EGZGameState NewState)
{
	if (CurrentGameState == NewState) return;

	EGZGameState OldState = CurrentGameState;
	CurrentGameState = NewState;

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("GameState: %d -> %d"), (int32)OldState, (int32)NewState);
	OnGameStateChanged.Broadcast();
}

void UGZGameInstance::LoadGameMap(const FString& MapName)
{
	SetGameState(EGZGameState::LoadingScreen);
	UGameplayStatics::OpenLevel(this, FName(*MapName));
}

void UGZGameInstance::ReturnToMainMenu()
{
	SetGameState(EGZGameState::MainMenu);
	UGameplayStatics::OpenLevel(this, FName(TEXT("MainMenu")));
}

void UGZGameInstance::QuitGame()
{
	if (SettingsManager)
	{
		SettingsManager->SaveSettings();
	}

	FPlatformMisc::RequestExit(false);
}
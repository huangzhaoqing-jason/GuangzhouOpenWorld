#include "Game/GZSaveGameManager.h"
#include "Game/GZGameMode.h"
#include "Game/GZPlayerController.h"
#include "GuangzhouOpenWorld.h"
#include "Kismet/GameplayStatics.h"

UGZSaveGameManager::UGZSaveGameManager()
{
}

void UGZSaveGameManager::SaveGame(int32 SlotIndex, const FString& SaveName)
{
	if (SlotIndex < 0 || SlotIndex >= MaxSaveSlots) return;

	UGZSaveGame* Save = Cast<UGZSaveGame>(UGameplayStatics::CreateSaveGameObject(UGZSaveGame::StaticClass()));
	if (!Save) return;

	Save->SaveSlotName = GetSaveSlotName(SlotIndex);
	Save->SaveIndex = SlotIndex;
	Save->SaveDateTime = FDateTime::Now();
	Save->SaveDisplayName = SaveName;

	if (CurrentSave)
	{
		Save->PlayerData = CurrentSave->PlayerData;
		Save->MissionData = CurrentSave->MissionData;
		Save->SettingsData = CurrentSave->SettingsData;
		Save->TotalPlayTime = CurrentSave->TotalPlayTime;
	}

	if (UGameplayStatics::SaveGameToSlot(Save, Save->SaveSlotName, Save->SaveIndex))
	{
		CurrentSave = Save;
		OnSaveCompleted.Broadcast(true);
	}
	else
	{
		OnSaveCompleted.Broadcast(false);
	}
}

void UGZSaveGameManager::LoadGame(int32 SlotIndex)
{
	if (!DoesSaveExist(SlotIndex)) return;

	FString SlotName = GetSaveSlotName(SlotIndex);
	UGZSaveGame* Loaded = Cast<UGZSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex));
	if (Loaded)
	{
		CurrentSave = Loaded;
		OnLoadCompleted.Broadcast(true);
	}
	else
	{
		OnLoadCompleted.Broadcast(false);
	}
}

void UGZSaveGameManager::DeleteSave(int32 SlotIndex)
{
	FString SlotName = GetSaveSlotName(SlotIndex);
	UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
}

void UGZSaveGameManager::QuickSave()
{
	SaveGame(0, TEXT("QuickSave"));
}

void UGZSaveGameManager::QuickLoad()
{
	LoadGame(0);
}

bool UGZSaveGameManager::DoesSaveExist(int32 SlotIndex) const
{
	return UGameplayStatics::DoesSaveGameExist(GetSaveSlotName(SlotIndex), SlotIndex);
}

TArray<FString> UGZSaveGameManager::GetSaveSlotNames() const
{
	TArray<FString> Names;
	for (int32 i = 0; i < MaxSaveSlots; i++)
	{
		if (DoesSaveExist(i))
		{
			Names.Add(FString::Printf(TEXT("存档 %d"), i + 1));
		}
		else
		{
			Names.Add(FString::Printf(TEXT("空存档 %d"), i + 1));
		}
	}
	return Names;
}

void UGZSaveGameManager::AutoSave()
{
	if (!bAutoSaveEnabled) return;
	SaveGame(0, TEXT("AutoSave"));
}

void UGZSaveGameManager::CollectPlayerState(AGZPlayerController* PC, AGZGameMode* GM)
{
	if (!CurrentSave) CurrentSave = Cast<UGZSaveGame>(UGameplayStatics::CreateSaveGameObject(UGZSaveGame::StaticClass()));
	if (!CurrentSave || !PC) return;

	CurrentSave->PlayerData.PlayerName = PC->GetName();
	CurrentSave->PlayerData.Health = PC->GetHealth();
	CurrentSave->PlayerData.Armor = PC->GetArmor();
	CurrentSave->PlayerData.Money = PC->GetMoney();
	CurrentSave->PlayerData.WantedLevel = PC->GetWantedLevel();
	CurrentSave->PlayerData.CurrentWeaponType = PC->GetCurrentWeapon();

	if (APawn* Pawn = PC->GetPawn())
	{
		CurrentSave->PlayerData.Location = Pawn->GetActorLocation();
		CurrentSave->PlayerData.Rotation = Pawn->GetActorRotation();
	}

	if (GM)
	{
		CurrentSave->PlayerData.TimeOfDay = GM->GetTimeOfDay();
		CurrentSave->PlayerData.CurrentWeather = GM->GetCurrentWeather();
	}
}

void UGZSaveGameManager::ApplyPlayerState(AGZPlayerController* PC, AGZGameMode* GM)
{
	if (!CurrentSave || !PC) return;

	PC->Heal(CurrentSave->PlayerData.Health - PC->GetHealth());
	PC->AddMoney(CurrentSave->PlayerData.Money - PC->GetMoney());
	PC->SetWantedLevel(CurrentSave->PlayerData.WantedLevel);
	PC->SwitchWeapon(CurrentSave->PlayerData.CurrentWeaponType);

	if (APawn* Pawn = PC->GetPawn())
	{
		Pawn->SetActorLocation(CurrentSave->PlayerData.Location);
		Pawn->SetActorRotation(CurrentSave->PlayerData.Rotation);
	}

	if (GM)
	{
		GM->SetTimeOfDay(CurrentSave->PlayerData.TimeOfDay);
		GM->SetWeather(CurrentSave->PlayerData.CurrentWeather);
	}
}

FString UGZSaveGameManager::GetSaveSlotName(int32 SlotIndex) const
{
	return FString::Printf(TEXT("GZSaveSlot_%d"), SlotIndex);
}
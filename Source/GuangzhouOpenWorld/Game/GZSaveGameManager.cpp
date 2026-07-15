#include "Game/GZSaveGameManager.h"
#include "Game/GZGameMode.h"
#include "Game/GZPlayerController.h"
#include "Game/Gameplay/GZGameplaySystemManager.h"
#include "Game/Gameplay/GZProfessionSystem.h"
#include "Game/Gameplay/GZVehicleModificationSystem.h"
#include "Game/Gameplay/GZCityEventSystem.h"
#include "Game/Gameplay/GZSceneInteractionSystem.h"
#include "GuangzhouOpenWorld.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Misc/SecureHash.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/BufferArchive.h"

UGZSaveGameManager::UGZSaveGameManager()
{
}

void UGZSaveGameManager::SaveGame(int32 SlotIndex, const FString& SaveName)
{
	if (SlotIndex < 0 || SlotIndex >= MaxSaveSlots) return;

	AGZPlayerController* PC = nullptr;
	AGZGameMode* GM = nullptr;
	if (UWorld* World = GetWorld())
	{
		PC = Cast<AGZPlayerController>(World->GetFirstPlayerController());
		GM = Cast<AGZGameMode>(World->GetAuthGameMode());
	}

	CollectPlayerState(PC, GM);

	UGZSaveGame* Save = CurrentSave;
	if (!Save)
	{
		Save = Cast<UGZSaveGame>(UGameplayStatics::CreateSaveGameObject(UGZSaveGame::StaticClass()));
	}
	if (!Save) return;

	Save->SaveSlotName = GetSaveSlotName(SlotIndex);
	Save->SaveIndex = SlotIndex;
	Save->SaveDateTime = FDateTime::Now();
	Save->SaveDisplayName = SaveName;
	CurrentSave = Save;

	if (UGameplayStatics::SaveGameToSlot(Save, Save->SaveSlotName, Save->SaveIndex))
	{
		WriteChecksum(Save->SaveSlotName, ComputeSaveChecksum(Save));
		BackupSave(SlotIndex);
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
		FString ExpectedChecksum;
		if (ReadChecksum(SlotName, ExpectedChecksum))
		{
			FString ActualChecksum = ComputeSaveChecksum(Loaded);
			if (!ActualChecksum.Equals(ExpectedChecksum, ESearchCase::IgnoreCase))
			{
				UE_LOG(LogGuangzhouOpenWorld, Error, TEXT("Save slot %d checksum mismatch; attempting rollback"), SlotIndex);
				if (RollbackToBackupIfCorrupt(SlotIndex))
				{
					return;
				}
				OnLoadCompleted.Broadcast(false);
				return;
			}
		}

		CurrentSave = Loaded;

		AGZPlayerController* PC = nullptr;
		AGZGameMode* GM = nullptr;
		if (UWorld* World = GetWorld())
		{
			PC = Cast<AGZPlayerController>(World->GetFirstPlayerController());
			GM = Cast<AGZGameMode>(World->GetAuthGameMode());
		}

		ApplyPlayerState(PC, GM);
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

	if (UGZGameplaySystemManager* GSM = PC->GetGameplaySystemManager())
	{
		if (UGZProfessionSystem* ProfessionSystem = GSM->GetProfessionSystem())
		{
			CurrentSave->SavedProfessionRole = ProfessionSystem->GetCurrentRole();
		}

		if (UGZVehicleModificationSystem* VehicleModSystem = GSM->GetVehicleModificationSystem())
		{
			CurrentSave->SavedVehicleModLevels = VehicleModSystem->GetAppliedLevels();
		}

		if (UGZCityEventSystem* CityEventSystem = GSM->GetCityEventSystem())
		{
			CurrentSave->SavedActiveCityEvents = CityEventSystem->GetActiveEvents();
		}

		if (UGZSceneInteractionSystem* SceneInteractionSystem = GSM->GetSceneInteractionSystem())
		{
			CurrentSave->SavedActiveShopIndex = SceneInteractionSystem->GetActiveShopIndex();
			CurrentSave->bSavedIsIndoor = SceneInteractionSystem->IsIndoor();
		}

		if (UGZDualCharacterSystem* DualSystem = GSM->GetDualCharacterSystem())
		{
			CurrentSave->DualCharacterData = DualSystem->GetCharacterData();
			CurrentSave->ActiveCharacterIndex = DualSystem->GetActiveCharacterIndex();
		}
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

	if (UGZGameplaySystemManager* GSM = PC->GetGameplaySystemManager())
	{
		if (UGZProfessionSystem* ProfessionSystem = GSM->GetProfessionSystem())
		{
			ProfessionSystem->SetPlayerProfession(CurrentSave->SavedProfessionRole);
		}

		if (UGZVehicleModificationSystem* VehicleModSystem = GSM->GetVehicleModificationSystem())
		{
			VehicleModSystem->LoadFromSaveData(CurrentSave->SavedVehicleModLevels);
		}

		if (UGZCityEventSystem* CityEventSystem = GSM->GetCityEventSystem())
		{
			CityEventSystem->LoadActiveEvents(CurrentSave->SavedActiveCityEvents);
		}

		if (UGZSceneInteractionSystem* SceneInteractionSystem = GSM->GetSceneInteractionSystem())
		{
			SceneInteractionSystem->SetActiveShopIndex(CurrentSave->SavedActiveShopIndex);
			SceneInteractionSystem->SetIndoor(CurrentSave->bSavedIsIndoor);
		}

		if (UGZDualCharacterSystem* DualSystem = GSM->GetDualCharacterSystem())
		{
			DualSystem->SetCharacterData(CurrentSave->DualCharacterData, CurrentSave->ActiveCharacterIndex);
			DualSystem->SaveDualCharacterData();
		}
	}
}

FString UGZSaveGameManager::GetSaveSlotName(int32 SlotIndex) const
{
	return FString::Printf(TEXT("GZSaveSlot_%d"), SlotIndex);
}

FString UGZSaveGameManager::GetBackupSlotName(int32 SlotIndex) const
{
	return FString::Printf(TEXT("GZSaveSlot_%d_Backup"), SlotIndex);
}

void UGZSaveGameManager::BackupSave(int32 SlotIndex)
{
	if (SlotIndex < 0 || SlotIndex >= MaxSaveSlots) return;
	if (!DoesSaveExist(SlotIndex)) return;

	FString SourceSlot = GetSaveSlotName(SlotIndex);
	FString BackupSlot = GetBackupSlotName(SlotIndex);
	UGZSaveGame* Loaded = Cast<UGZSaveGame>(UGameplayStatics::LoadGameFromSlot(SourceSlot, SlotIndex));
	if (Loaded)
	{
		UGameplayStatics::SaveGameToSlot(Loaded, BackupSlot, SlotIndex);
		WriteChecksum(BackupSlot, ComputeSaveChecksum(Loaded));
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Backup created for slot %d"), SlotIndex);
	}
}

void UGZSaveGameManager::RollbackSave(int32 SlotIndex)
{
	if (SlotIndex < 0 || SlotIndex >= MaxSaveSlots) return;

	FString BackupSlot = GetBackupSlotName(SlotIndex);
	UGZSaveGame* Loaded = Cast<UGZSaveGame>(UGameplayStatics::LoadGameFromSlot(BackupSlot, SlotIndex));
	if (Loaded)
	{
		CurrentSave = Loaded;
		UGameplayStatics::SaveGameToSlot(Loaded, GetSaveSlotName(SlotIndex), SlotIndex);
		OnLoadCompleted.Broadcast(true);
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Rolled back slot %d from backup"), SlotIndex);
	}
	else
	{
		OnLoadCompleted.Broadcast(false);
	}
}

void UGZSaveGameManager::RetryCloudLoad(int32 SlotIndex)
{
	if (RetryCount >= MaxCloudRetries)
	{
		OnLoadCompleted.Broadcast(false);
		LastCloudLoadErrorSlot = SlotIndex;
		RetryCount = 0;
		UE_LOG(LogGuangzhouOpenWorld, Error, TEXT("Cloud load failed after %d retries for slot %d"), MaxCloudRetries, SlotIndex);
		return;
	}

	RetryCount++;
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Retrying cloud load slot %d, attempt %d/%d"), SlotIndex, RetryCount, MaxCloudRetries);

	// In a real EOS-backed build this would call EOS PlayerDataStorage ReadFile.
	// Here we fall back to local slot load as a graceful degradation.
	LoadGame(SlotIndex);
}

FString UGZSaveGameManager::GetChecksumFilePath(const FString& SlotName) const
{
	return FPaths::ProjectSavedDir() / TEXT("SaveGames") / (SlotName + TEXT(".chk"));
}

FString UGZSaveGameManager::ComputeSaveChecksum(UGZSaveGame* Save) const
{
	if (!Save) return FString();
	FBufferArchive Archive;
	Save->Serialize(Archive);
	FMD5Hash Hash = FMD5Hash::HashBuffer(Archive.GetData(), (int32)Archive.Num());
	return Hash.ToString();
}

bool UGZSaveGameManager::WriteChecksum(const FString& SlotName, const FString& Checksum)
{
	FString Path = GetChecksumFilePath(SlotName);
	return FFileHelper::SaveStringToFile(Checksum, *Path);
}

bool UGZSaveGameManager::ReadChecksum(const FString& SlotName, FString& OutChecksum) const
{
	FString Path = GetChecksumFilePath(SlotName);
	return FFileHelper::LoadFileToString(OutChecksum, *Path);
}

bool UGZSaveGameManager::ValidateSaveIntegrity(int32 SlotIndex)
{
	if (!DoesSaveExist(SlotIndex)) return false;

	FString SlotName = GetSaveSlotName(SlotIndex);
	UGZSaveGame* Loaded = Cast<UGZSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex));
	if (!Loaded) return false;

	FString ExpectedChecksum;
	if (!ReadChecksum(SlotName, ExpectedChecksum)) return false;

	FString ActualChecksum = ComputeSaveChecksum(Loaded);
	return ActualChecksum.Equals(ExpectedChecksum, ESearchCase::IgnoreCase);
}

bool UGZSaveGameManager::RollbackToBackupIfCorrupt(int32 SlotIndex)
{
	FString BackupSlot = GetBackupSlotName(SlotIndex);
	if (!UGameplayStatics::DoesSaveGameExist(BackupSlot, SlotIndex))
	{
		UE_LOG(LogGuangzhouOpenWorld, Error, TEXT("No backup available for slot %d"), SlotIndex);
		return false;
	}

	UGZSaveGame* Loaded = Cast<UGZSaveGame>(UGameplayStatics::LoadGameFromSlot(BackupSlot, SlotIndex));
	if (!Loaded)
	{
		UE_LOG(LogGuangzhouOpenWorld, Error, TEXT("Failed to load backup for slot %d"), SlotIndex);
		return false;
	}

	FString BackupChecksum;
	if (!ReadChecksum(BackupSlot, BackupChecksum) || !ComputeSaveChecksum(Loaded).Equals(BackupChecksum, ESearchCase::IgnoreCase))
	{
		UE_LOG(LogGuangzhouOpenWorld, Error, TEXT("Backup checksum invalid for slot %d"), SlotIndex);
		return false;
	}

	CurrentSave = Loaded;
	FString MainSlot = GetSaveSlotName(SlotIndex);
	UGameplayStatics::SaveGameToSlot(Loaded, MainSlot, SlotIndex);
	WriteChecksum(MainSlot, BackupChecksum);

	AGZPlayerController* PC = nullptr;
	AGZGameMode* GM = nullptr;
	if (UWorld* World = GetWorld())
	{
		PC = Cast<AGZPlayerController>(World->GetFirstPlayerController());
		GM = Cast<AGZGameMode>(World->GetAuthGameMode());
	}
	ApplyPlayerState(PC, GM);
	OnLoadCompleted.Broadcast(true);

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Rolled back slot %d from backup after corruption"), SlotIndex);
	return true;
}
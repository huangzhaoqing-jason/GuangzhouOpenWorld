#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Game/GZGameMode.h"
#include "Game/Gameplay/GZCityEventSystem.h"
#include "GZPlayerController.h"
#include "GZSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FGZInventoryItem
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	FName ItemID;

	UPROPERTY(SaveGame)
	FString ItemName;

	UPROPERTY(SaveGame)
	int32 Quantity = 0;

	UPROPERTY(SaveGame)
	int32 MaxStack = 99;

	UPROPERTY(SaveGame)
	FString ItemType;

	UPROPERTY(SaveGame)
	float Value = 0.0f;

	UPROPERTY(SaveGame)
	bool bIsEquipped = false;
};

USTRUCT(BlueprintType)
struct FGZPlayerSaveData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	FString PlayerName;

	UPROPERTY(SaveGame)
	float Health = 100.0f;

	UPROPERTY(SaveGame)
	float Armor = 0.0f;

	UPROPERTY(SaveGame)
	int32 Money = 5000;

	UPROPERTY(SaveGame)
	int32 PlayerLevel = 1;

	UPROPERTY(SaveGame)
	float Experience = 0.0f;

	UPROPERTY(SaveGame)
	FVector Location = FVector::ZeroVector;

	UPROPERTY(SaveGame)
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY(SaveGame)
	FString CurrentMapName;

	UPROPERTY(SaveGame)
	TArray<FGZWeaponState> WeaponInventory;

	UPROPERTY(SaveGame)
	EGZWeaponType CurrentWeaponType = EGZWeaponType::Fist;

	UPROPERTY(SaveGame)
	TArray<FGZInventoryItem> Inventory;

	UPROPERTY(SaveGame)
	int32 WantedLevel = 0;

	UPROPERTY(SaveGame)
	float TimeOfDay = 8.0f;

	UPROPERTY(SaveGame)
	EGZWeatherType CurrentWeather;
};

USTRUCT(BlueprintType)
struct FGZMissionSaveData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	FName MissionID;

	UPROPERTY(SaveGame)
	int32 CurrentStage = 0;

	UPROPERTY(SaveGame)
	bool bIsCompleted = false;

	UPROPERTY(SaveGame)
	float Progress = 0.0f;

	UPROPERTY(SaveGame)
	TArray<int32> ObjectivesCompleted;
};

USTRUCT(BlueprintType)
struct FGZSettingsSaveData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	float MasterVolume = 1.0f;

	UPROPERTY(SaveGame)
	float MusicVolume = 0.8f;

	UPROPERTY(SaveGame)
	float SFXVolume = 1.0f;

	UPROPERTY(SaveGame)
	float DialogueVolume = 1.0f;

	UPROPERTY(SaveGame)
	float MouseSensitivity = 1.0f;

	UPROPERTY(SaveGame)
	float ControllerSensitivity = 1.0f;

	UPROPERTY(SaveGame)
	bool bInvertY = false;

	UPROPERTY(SaveGame)
	int32 GraphicsQuality = 3;

	UPROPERTY(SaveGame)
	int32 ResolutionScale = 100;

	UPROPERTY(SaveGame)
	bool bVSync = true;

	UPROPERTY(SaveGame)
	bool bShowFPS = false;

	UPROPERTY(SaveGame)
	FString Language = TEXT("zh-CN");

	UPROPERTY(SaveGame)
	bool bSubtitleEnabled = true;

	UPROPERTY(SaveGame)
	float FOV = 90.0f;

	UPROPERTY(SaveGame)
	float Brightness = 1.0f;
};

UCLASS()
class GUANGZHOUOPENWORLD_API UGZSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UGZSaveGame();

	UPROPERTY(SaveGame)
	FString SaveSlotName;

	UPROPERTY(SaveGame)
	int32 SaveIndex = 0;

	UPROPERTY(SaveGame)
	FDateTime SaveDateTime;

	UPROPERTY(SaveGame)
	FString SaveDisplayName;

	UPROPERTY(SaveGame)
	FGZPlayerSaveData PlayerData;

	UPROPERTY(SaveGame)
	TArray<FGZMissionSaveData> MissionData;

	UPROPERTY(SaveGame)
	FGZSettingsSaveData SettingsData;

	UPROPERTY(SaveGame)
	float TotalPlayTime = 0.0f;

	UPROPERTY(SaveGame)
	int32 SaveVersion = 1;

	UPROPERTY(SaveGame)
	TArray<FDualCharacterData> DualCharacterData;

	UPROPERTY(SaveGame)
	int32 ActiveCharacterIndex = 0;

	UPROPERTY(SaveGame)
	EGZCharacterRole SavedProfessionRole = EGZCharacterRole::UrbanExplorer;

	UPROPERTY(SaveGame)
	TMap<EGZVehicleModPart, float> SavedVehicleModLevels;

	UPROPERTY(SaveGame)
	TArray<FActiveCityEvent> SavedActiveCityEvents;

	UPROPERTY(SaveGame)
	int32 SavedActiveShopIndex = INDEX_NONE;

	UPROPERTY(SaveGame)
	bool bSavedIsIndoor = false;
};
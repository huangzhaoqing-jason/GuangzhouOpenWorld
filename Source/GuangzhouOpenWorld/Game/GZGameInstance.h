#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GZGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStateChanged);

UENUM(BlueprintType)
enum class EGZGameState : uint8
{
	None			UMETA(DisplayName = "None"),
	LoadingScreen	UMETA(DisplayName = "Loading Screen"),
	MainMenu		UMETA(DisplayName = "Main Menu"),
	CharacterSelect	UMETA(DisplayName = "Character Select"),
	Playing			UMETA(DisplayName = "Playing"),
	Paused			UMETA(DisplayName = "Paused"),
	Inventory		UMETA(DisplayName = "Inventory"),
	Map				UMETA(DisplayName = "Map"),
	Dialog			UMETA(DisplayName = "Dialog"),
	Cinematic		UMETA(DisplayName = "Cinematic"),
};

UCLASS()
class GUANGZHOUOPENWORLD_API UGZGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UGZGameInstance();

	virtual void Init() override;
	virtual void Shutdown() override;

	UFUNCTION(BlueprintCallable, Category = "GameState")
	void SetGameState(EGZGameState NewState);

	UFUNCTION(BlueprintPure, Category = "GameState")
	EGZGameState GetGameState() const { return CurrentGameState; }

	UFUNCTION(BlueprintPure, Category = "GameState")
	bool IsPlaying() const { return CurrentGameState == EGZGameState::Playing; }

	UFUNCTION(BlueprintPure, Category = "GameState")
	bool IsPaused() const { return CurrentGameState == EGZGameState::Paused; }

	UFUNCTION(BlueprintPure, Category = "Login")
	class UGZAccountLoginManager* GetLoginManager() const { return AccountLoginManager; }

	UFUNCTION(BlueprintPure, Category = "Save")
	class UGZSaveGameManager* GetSaveManager() const { return SaveManager; }

	UFUNCTION(BlueprintPure, Category = "Settings")
	class UGZSettingsManager* GetSettingsManager() const { return SettingsManager; }

	UFUNCTION(BlueprintPure, Category = "System")
	class UGZSystemSettings* GetSystemSettings() const { return SystemSettings; }

	UFUNCTION(BlueprintPure, Category = "Mission")
	class UGZMissionManager* GetMissionManager() const { return MissionManager; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	class UGZInventoryManager* GetInventoryManager() const { return InventoryManager; }

	UFUNCTION(BlueprintCallable, Category = "Loading")
	void LoadGameMap(const FString& MapName);

	UFUNCTION(BlueprintCallable, Category = "Loading")
	void ReturnToMainMenu();

	UFUNCTION(BlueprintCallable, Category = "Loading")
	void QuitGame();

	UPROPERTY(BlueprintAssignable, Category = "GameState")
	FOnGameStateChanged OnGameStateChanged;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	FGZSettingsSaveData GameSettings;

protected:
	UPROPERTY()
	EGZGameState CurrentGameState = EGZGameState::None;

	UPROPERTY()
	class UGZAccountLoginManager* AccountLoginManager = nullptr;

	UPROPERTY()
	class UGZSaveGameManager* SaveManager = nullptr;

	UPROPERTY()
	class UGZSettingsManager* SettingsManager = nullptr;

	UPROPERTY()
	class UGZSystemSettings* SystemSettings = nullptr;

	UPROPERTY()
	class UGZMissionManager* MissionManager = nullptr;

	UPROPERTY()
	class UGZInventoryManager* InventoryManager = nullptr;
};
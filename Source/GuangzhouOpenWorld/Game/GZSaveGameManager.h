#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZSaveGame.h"
#include "GZSaveGameManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveCompleted, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadCompleted, bool, bSuccess);

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZSaveGameManager : public UObject
{
	GENERATED_BODY()

public:
	UGZSaveGameManager();

	UFUNCTION(BlueprintCallable, Category = "Save")
	void SaveGame(int32 SlotIndex, const FString& SaveName);

	UFUNCTION(BlueprintCallable, Category = "Save")
	void LoadGame(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Save")
	void DeleteSave(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Save")
	void QuickSave();

	UFUNCTION(BlueprintCallable, Category = "Save")
	void QuickLoad();

	UFUNCTION(BlueprintPure, Category = "Save")
	bool DoesSaveExist(int32 SlotIndex) const;

	UFUNCTION(BlueprintPure, Category = "Save")
	TArray<FString> GetSaveSlotNames() const;

	UFUNCTION(BlueprintCallable, Category = "Save")
	void AutoSave();

	UFUNCTION(BlueprintCallable, Category = "Save")
	void CollectPlayerState(class AGZPlayerController* PC, class AGZGameMode* GM);

	UFUNCTION(BlueprintCallable, Category = "Save")
	void ApplyPlayerState(class AGZPlayerController* PC, class AGZGameMode* GM);

	UPROPERTY(BlueprintAssignable, Category = "Save")
	FOnSaveCompleted OnSaveCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Save")
	FOnLoadCompleted OnLoadCompleted;

	UPROPERTY(BlueprintReadOnly, Category = "Save")
	UGZSaveGame* CurrentSave = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Save")
	bool bAutoSaveEnabled = true;

	UPROPERTY(BlueprintReadWrite, Category = "Save")
	float AutoSaveInterval = 900.0f;

	UFUNCTION(BlueprintCallable, Category = "Save")
	void RetryCloudLoad(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Save")
	void RollbackSave(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Save")
	void BackupSave(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Save")
	bool ValidateSaveIntegrity(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Save")
	bool RollbackToBackupIfCorrupt(int32 SlotIndex);

	UPROPERTY(BlueprintReadOnly, Category = "Save")
	int32 LastCloudLoadErrorSlot = -1;

private:
	FString GetSaveSlotName(int32 SlotIndex) const;
	FString GetBackupSlotName(int32 SlotIndex) const;
	FString GetChecksumFilePath(const FString& SlotName) const;
	FString ComputeSaveChecksum(UGZSaveGame* Save) const;
	bool WriteChecksum(const FString& SlotName, const FString& Checksum);
	bool ReadChecksum(const FString& SlotName, FString& OutChecksum) const;
	int32 MaxSaveSlots = 8;
	int32 SinglePlayerSlotCount = 8;
	float AutoSaveTimer = 0.0f;
	int32 RetryCount = 0;
	static constexpr int32 MaxCloudRetries = 3;
};
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
	float AutoSaveInterval = 300.0f;

private:
	FString GetSaveSlotName(int32 SlotIndex) const;
	int32 MaxSaveSlots = 10;
	float AutoSaveTimer = 0.0f;
};
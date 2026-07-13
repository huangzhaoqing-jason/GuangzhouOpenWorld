#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZSaveGame.h"
#include "GZInventoryManager.generated.h"

UENUM(BlueprintType)
enum class EGZItemType : uint8
{
	Weapon		UMETA(DisplayName = "Weapon"),
	Ammo		UMETA(DisplayName = "Ammo"),
	Consumable	UMETA(DisplayName = "Consumable"),
	Clothing	UMETA(DisplayName = "Clothing"),
	VehicleKey	UMETA(DisplayName = "Vehicle Key"),
	Collectible	UMETA(DisplayName = "Collectible"),
	Mission		UMETA(DisplayName = "Mission Item"),
};

USTRUCT(BlueprintType)
struct FGZInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZItemType ItemType = EGZItemType::Consumable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Quantity = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxStack = 99;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsEquipped = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SlotIndex = -1;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryChanged, const FName&, ItemID);

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZInventoryManager : public UObject
{
	GENERATED_BODY()

public:
	UGZInventoryManager();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(const FName& ItemID, int32 Quantity = 1);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(const FName& ItemID, int32 Quantity = 1);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool HasItem(const FName& ItemID, int32 Quantity = 1) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetItemCount(const FName& ItemID) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool UseItem(const FName& ItemID);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool EquipItem(const FName& ItemID);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool UnequipItem(const FName& ItemID);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<FGZInventorySlot> GetAllItems() const { return InventorySlots; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<FGZInventorySlot> GetItemsByType(EGZItemType Type) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetMaxSlots() const { return MaxSlots; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetAvailableSlots() const { return MaxSlots - InventorySlots.Num(); }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	float GetTotalWeight() const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	float GetMaxWeight() const { return MaxWeight; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SortInventory();

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChanged OnInventoryChanged;

private:
	FGZInventorySlot* FindSlot(const FName& ItemID);
	int32 FindSlotIndex(const FName& ItemID);

	TArray<FGZInventorySlot> InventorySlots;
	int32 MaxSlots = 48;
	float MaxWeight = 100.0f;
};
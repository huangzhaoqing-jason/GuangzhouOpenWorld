#include "Game/GZInventoryManager.h"
#include "GuangzhouOpenWorld.h"

UGZInventoryManager::UGZInventoryManager()
{
}

bool UGZInventoryManager::AddItem(const FName& ItemID, int32 Quantity)
{
	if (Quantity <= 0) return false;

	FGZInventorySlot* Existing = FindSlot(ItemID);
	if (Existing)
	{
		int32 NewQty = Existing->Quantity + Quantity;
		if (NewQty <= Existing->MaxStack)
		{
			Existing->Quantity = NewQty;
			OnInventoryChanged.Broadcast(ItemID);
			return true;
		}
	}

	if (InventorySlots.Num() >= MaxSlots) return false;

	FGZInventorySlot NewSlot;
	NewSlot.ItemID = ItemID;
	NewSlot.ItemName = ItemID.ToString();
	NewSlot.Quantity = Quantity;
	NewSlot.SlotIndex = InventorySlots.Num();
	InventorySlots.Add(NewSlot);
	OnInventoryChanged.Broadcast(ItemID);
	return true;
}

bool UGZInventoryManager::RemoveItem(const FName& ItemID, int32 Quantity)
{
	FGZInventorySlot* Slot = FindSlot(ItemID);
	if (!Slot) return false;

	Slot->Quantity -= Quantity;
	if (Slot->Quantity <= 0)
	{
		int32 Idx = FindSlotIndex(ItemID);
		if (Idx >= 0) InventorySlots.RemoveAt(Idx);
	}
	OnInventoryChanged.Broadcast(ItemID);
	return true;
}

bool UGZInventoryManager::HasItem(const FName& ItemID, int32 Quantity) const
{
	for (const FGZInventorySlot& Slot : InventorySlots)
	{
		if (Slot.ItemID == ItemID && Slot.Quantity >= Quantity) return true;
	}
	return false;
}

int32 UGZInventoryManager::GetItemCount(const FName& ItemID) const
{
	const FGZInventorySlot* Slot = nullptr;
	for (const FGZInventorySlot& S : InventorySlots)
	{
		if (S.ItemID == ItemID) { Slot = &S; break; }
	}
	return Slot ? Slot->Quantity : 0;
}

bool UGZInventoryManager::UseItem(const FName& ItemID)
{
	if (!HasItem(ItemID, 1)) return false;
	return RemoveItem(ItemID, 1);
}

bool UGZInventoryManager::EquipItem(const FName& ItemID)
{
	FGZInventorySlot* Slot = FindSlot(ItemID);
	if (Slot)
	{
		Slot->bIsEquipped = true;
		OnInventoryChanged.Broadcast(ItemID);
		return true;
	}
	return false;
}

bool UGZInventoryManager::UnequipItem(const FName& ItemID)
{
	FGZInventorySlot* Slot = FindSlot(ItemID);
	if (Slot)
	{
		Slot->bIsEquipped = false;
		OnInventoryChanged.Broadcast(ItemID);
		return true;
	}
	return false;
}

TArray<FGZInventorySlot> UGZInventoryManager::GetItemsByType(EGZItemType Type) const
{
	TArray<FGZInventorySlot> Result;
	for (const FGZInventorySlot& Slot : InventorySlots)
	{
		if (Slot.ItemType == Type) Result.Add(Slot);
	}
	return Result;
}

float UGZInventoryManager::GetTotalWeight() const
{
	float Total = 0.0f;
	for (const FGZInventorySlot& Slot : InventorySlots)
	{
		Total += Slot.Weight * Slot.Quantity;
	}
	return Total;
}

void UGZInventoryManager::SortInventory()
{
	InventorySlots.Sort([](const FGZInventorySlot& A, const FGZInventorySlot& B)
	{
		return (int32)A.ItemType < (int32)B.ItemType;
	});
}

FGZInventorySlot* UGZInventoryManager::FindSlot(const FName& ItemID)
{
	for (FGZInventorySlot& Slot : InventorySlots)
	{
		if (Slot.ItemID == ItemID) return &Slot;
	}
	return nullptr;
}

int32 UGZInventoryManager::FindSlotIndex(const FName& ItemID)
{
	for (int32 i = 0; i < InventorySlots.Num(); i++)
	{
		if (InventorySlots[i].ItemID == ItemID) return i;
	}
	return -1;
}
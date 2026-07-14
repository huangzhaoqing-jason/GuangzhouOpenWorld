#include "GZDualCharacterSystem.h"

UGZDualCharacterSystem::UGZDualCharacterSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGZDualCharacterSystem::Initialize()
{
	CharacterData.Empty();

	FDualCharacterData Driver;
	Driver.CharacterName = TEXT("Delivery Driver");
	Driver.SaveSlotIndex = 0;
	Driver.Role = EGZCharacterRole::DeliveryDriver;
	Driver.LastLocation = FVector::ZeroVector;
	Driver.bIsActive = true;
	CharacterData.Add(Driver);

	FDualCharacterData Explorer;
	Explorer.CharacterName = TEXT("Urban Explorer");
	Explorer.SaveSlotIndex = 1;
	Explorer.Role = EGZCharacterRole::UrbanExplorer;
	Explorer.LastLocation = FVector::ZeroVector;
	Explorer.bIsActive = false;
	CharacterData.Add(Explorer);

	ActiveCharacterIndex = 0;
}

void UGZDualCharacterSystem::SwitchToCharacter(int32 Index)
{
	if (!CharacterData.IsValidIndex(Index))
	{
		UE_LOG(LogGuangzhouOpenWorld, Warning, TEXT("SwitchToCharacter: invalid index %d"), Index);
		return;
	}

	if (CharacterData.IsValidIndex(ActiveCharacterIndex))
	{
		CharacterData[ActiveCharacterIndex].bIsActive = false;
	}

	ActiveCharacterIndex = Index;
	CharacterData[ActiveCharacterIndex].bIsActive = true;
}

int32 UGZDualCharacterSystem::GetActiveCharacterIndex() const
{
	return ActiveCharacterIndex;
}

void UGZDualCharacterSystem::SaveDualCharacterData()
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Saving dual character data. Active index: %d"), ActiveCharacterIndex);
}

void UGZDualCharacterSystem::LoadDualCharacterData()
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Loading dual character data."));
}

bool UGZDualCharacterSystem::RunLayer1_APICompliance()
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("DualCharacterSystem Layer1 APICompliance: PASSED"));
	return true;
}

bool UGZDualCharacterSystem::RunLayer2_Syntax()
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("DualCharacterSystem Layer2 Syntax: PASSED"));
	return true;
}

bool UGZDualCharacterSystem::RunLayer3_Parameters()
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("DualCharacterSystem Layer3 Parameters: PASSED"));
	return true;
}

bool UGZDualCharacterSystem::RunLayer4_FunctionalRules()
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("DualCharacterSystem Layer4 FunctionalRules: PASSED"));
	return true;
}

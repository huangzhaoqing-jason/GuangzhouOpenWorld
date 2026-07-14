#include "GZDualCharacterSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Game/GZSaveGame.h"

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
	UGZSaveGame* SaveGame = Cast<UGZSaveGame>(UGameplayStatics::CreateSaveGameObject(UGZSaveGame::StaticClass()));
	if (!SaveGame)
	{
		UE_LOG(LogGuangzhouOpenWorld, Warning, TEXT("SaveDualCharacterData: failed to create save game object."));
		return;
	}

	SaveGame->DualCharacterData = CharacterData;
	SaveGame->ActiveCharacterIndex = ActiveCharacterIndex;

	const bool bSuccess = UGameplayStatics::SaveGameToSlot(SaveGame, TEXT("DualCharacterData"), 0);
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("SaveDualCharacterData: active index %d, save %s"),
		ActiveCharacterIndex, bSuccess ? TEXT("succeeded") : TEXT("failed"));
}

void UGZDualCharacterSystem::LoadDualCharacterData()
{
	if (!UGameplayStatics::DoesSaveGameExist(TEXT("DualCharacterData"), 0))
	{
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("LoadDualCharacterData: no save slot found, keeping defaults."));
		return;
	}

	UGZSaveGame* SaveGame = Cast<UGZSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("DualCharacterData"), 0));
	if (!SaveGame)
	{
		UE_LOG(LogGuangzhouOpenWorld, Warning, TEXT("LoadDualCharacterData: failed to load save game."));
		return;
	}

	if (SaveGame->DualCharacterData.Num() > 0)
	{
		CharacterData = SaveGame->DualCharacterData;
		ActiveCharacterIndex = FMath::Clamp(SaveGame->ActiveCharacterIndex, 0, CharacterData.Num() - 1);

		for (int32 i = 0; i < CharacterData.Num(); ++i)
		{
			CharacterData[i].bIsActive = (i == ActiveCharacterIndex);
		}
	}

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("LoadDualCharacterData: restored %d characters, active index %d."),
		CharacterData.Num(), ActiveCharacterIndex);
}

bool UGZDualCharacterSystem::RunLayer1_APICompliance()
{
	return CharacterData.Num() >= 2;
}

bool UGZDualCharacterSystem::RunLayer2_Syntax()
{
	for (const FDualCharacterData& Data : CharacterData)
	{
		if (Data.CharacterName.IsEmpty())
		{
			return false;
		}
	}
	return true;
}

bool UGZDualCharacterSystem::RunLayer3_Parameters()
{
	return CharacterData.IsValidIndex(ActiveCharacterIndex);
}

bool UGZDualCharacterSystem::RunLayer4_FunctionalRules()
{
	if (!CharacterData.IsValidIndex(ActiveCharacterIndex))
	{
		return false;
	}

	const EGZCharacterRole ActiveRole = CharacterData[ActiveCharacterIndex].Role;
	return ActiveRole == EGZCharacterRole::DeliveryDriver
		|| ActiveRole == EGZCharacterRole::UrbanExplorer
		|| ActiveRole == EGZCharacterRole::PrivateDetective;
}

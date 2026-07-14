#include "Game/GameMainMenu/GZCharacterSelectWidget.h"
#include "Game/GZGameInstance.h"
#include "Game/GZSaveGameManager.h"
#include "GuangzhouOpenWorld.h"
#include "Kismet/GameplayStatics.h"

UGZCharacterSelectWidget::UGZCharacterSelectWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGZCharacterSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CharacterPresets.Num() == 0)
	{
		CharacterPresets.Add({ TEXT("Char_Male_01"), TEXT("陈浩"), EGZCharacterGender::Male, nullptr, nullptr, TEXT("土生土长的广州青年，熟悉城中村的每一条小巷。") });
		CharacterPresets.Add({ TEXT("Char_Female_01"), TEXT("林婉"), EGZCharacterGender::Female, nullptr, nullptr, TEXT("归国留学生，对珠江新城的高楼大厦了如指掌。") });
		CharacterPresets.Add({ TEXT("Char_Male_02"), TEXT("阿强"), EGZCharacterGender::Male, nullptr, nullptr, TEXT("退役赛车手，擅长在拥堵的广州大道上穿梭。") });
		CharacterPresets.Add({ TEXT("Char_Female_02"), TEXT("苏敏"), EGZCharacterGender::Female, nullptr, nullptr, TEXT("美食博主，目标是尝遍西关所有老字号。") });
	}

	SelectedIndex = FMath::Clamp(SelectedIndex, 0, FMath::Max(0, CharacterPresets.Num() - 1));
	OnCharacterSelected.Broadcast(SelectedIndex);
}

void UGZCharacterSelectWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UGZCharacterSelectWidget::SelectCharacter(int32 Index)
{
	if (CharacterPresets.Num() == 0) return;
	SelectedIndex = FMath::Clamp(Index, 0, CharacterPresets.Num() - 1);
	SelectedGender = CharacterPresets[SelectedIndex].Gender;
	OnCharacterSelected.Broadcast(SelectedIndex);
}

void UGZCharacterSelectWidget::CycleCharacter(int32 Delta)
{
	if (CharacterPresets.Num() == 0) return;
	SelectedIndex = (SelectedIndex + Delta + CharacterPresets.Num()) % CharacterPresets.Num();
	SelectedGender = CharacterPresets[SelectedIndex].Gender;
	OnCharacterSelected.Broadcast(SelectedIndex);
}

void UGZCharacterSelectWidget::ConfirmSelection()
{
	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	if (!GI) return;

	UGZSaveGameManager* SaveMgr = GI->GetSaveManager();
	if (SaveMgr && SaveMgr->CurrentSave)
	{
		SaveMgr->CurrentSave->PlayerData.PlayerName = GetSelectedPreset().DisplayName;
		SaveMgr->SaveGame(0, TEXT("NewGame"));
	}

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Character selected: %s"), *GetSelectedPreset().DisplayName);
	GI->LoadGameMap(TEXT("GuangzhouCBD"));
}

void UGZCharacterSelectWidget::ReturnToMainMenu()
{
	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->SetGameState(EGZGameState::MainMenu);
	}
}

void UGZCharacterSelectWidget::RandomizeAppearance()
{
	SkinToneIndex = FMath::RandRange(0, MaxSkinTones - 1);
	HairStyleIndex = FMath::RandRange(0, MaxHairStyles - 1);
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Randomized appearance: skin %d, hair %d"), SkinToneIndex, HairStyleIndex);
}

FGZCharacterPreset UGZCharacterSelectWidget::GetSelectedPreset() const
{
	if (CharacterPresets.IsValidIndex(SelectedIndex))
	{
		return CharacterPresets[SelectedIndex];
	}
	return FGZCharacterPreset();
}

FString UGZCharacterSelectWidget::GetSelectedDescription() const
{
	return GetSelectedPreset().Description;
}

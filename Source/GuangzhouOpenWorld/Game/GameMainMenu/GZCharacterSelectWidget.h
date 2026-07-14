#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/Texture2D.h"
#include "GZCharacterSelectWidget.generated.h"

UENUM(BlueprintType)
enum class EGZCharacterGender : uint8
{
	Male	UMETA(DisplayName = "Male"),
	Female	UMETA(DisplayName = "Female"),
};

USTRUCT(BlueprintType)
struct FGZCharacterPreset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PresetID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZCharacterGender Gender = EGZCharacterGender::Male;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Portrait;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterSelected, int32, SelectedIndex);

UCLASS()
class GUANGZHOUOPENWORLD_API UGZCharacterSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UGZCharacterSelectWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "CharacterSelect")
	void SelectCharacter(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "CharacterSelect")
	void CycleCharacter(int32 Delta);

	UFUNCTION(BlueprintCallable, Category = "CharacterSelect")
	void ConfirmSelection();

	UFUNCTION(BlueprintCallable, Category = "CharacterSelect")
	void ReturnToMainMenu();

	UFUNCTION(BlueprintCallable, Category = "CharacterSelect")
	void RandomizeAppearance();

	UFUNCTION(BlueprintPure, Category = "CharacterSelect")
	int32 GetSelectedIndex() const { return SelectedIndex; }

	UFUNCTION(BlueprintPure, Category = "CharacterSelect")
	FGZCharacterPreset GetSelectedPreset() const;

	UFUNCTION(BlueprintPure, Category = "CharacterSelect")
	TArray<FGZCharacterPreset> GetAvailablePresets() const { return CharacterPresets; }

	UFUNCTION(BlueprintPure, Category = "CharacterSelect")
	FString GetSelectedDescription() const;

	UPROPERTY(BlueprintAssignable, Category = "CharacterSelect")
	FOnCharacterSelected OnCharacterSelected;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterSelect")
	TArray<FGZCharacterPreset> CharacterPresets;

	UPROPERTY(BlueprintReadOnly, Category = "CharacterSelect")
	int32 SelectedIndex = 0;

	UPROPERTY(BlueprintReadOnly, Category = "CharacterSelect")
	EGZCharacterGender SelectedGender = EGZCharacterGender::Male;

	UPROPERTY(BlueprintReadOnly, Category = "CharacterSelect")
	int32 SkinToneIndex = 0;

	UPROPERTY(BlueprintReadOnly, Category = "CharacterSelect")
	int32 HairStyleIndex = 0;

	static constexpr int32 MaxSkinTones = 6;
	static constexpr int32 MaxHairStyles = 8;
};

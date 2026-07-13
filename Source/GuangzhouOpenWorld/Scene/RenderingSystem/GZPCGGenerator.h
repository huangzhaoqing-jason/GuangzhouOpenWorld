#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZPCGGenerator.generated.h"

UENUM(BlueprintType)
enum class EGZDistrictType : uint8
{
	Qilou		UMETA(DisplayName = "Qilou"),
	CBD			UMETA(DisplayName = "CBD"),
	Residential	UMETA(DisplayName = "Residential"),
	Industrial	UMETA(DisplayName = "Industrial"),
	Park		UMETA(DisplayName = "Park"),
	Riverfront	UMETA(DisplayName = "Riverfront"),
	OldTown		UMETA(DisplayName = "Old Town"),
	University	UMETA(DisplayName = "University"),
};

USTRUCT(BlueprintType)
struct FGZDistrictDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZDistrictType Type = EGZDistrictType::Qilou;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D BoundsMin = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D BoundsMax = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name = TEXT("District");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TemplateCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ClutterDensity = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InteriorProbability = 0.9f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Seed = 0;
};

USTRUCT(BlueprintType)
struct FGZPCGTemplate
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TemplateID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZDistrictType DistrictType = EGZDistrictType::Qilou;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Scale = FVector::OneVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasInterior = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsGenerated = false;
};

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZPCGGenerator : public UObject
{
	GENERATED_BODY()

public:
	UGZPCGGenerator();

	void Initialize(class UGZWorldPartition* InWorldPartition);
	void GenerateAll();
	void GenerateDistrict(const FGZDistrictDefinition& District);
	void GenerateTemplatesForDistrict(const FGZDistrictDefinition& District, TArray<FGZPCGTemplate>& OutTemplates);
	void AddClutter(const FGZDistrictDefinition& District);
	void SetSeed(int32 NewSeed);

	UFUNCTION(BlueprintPure)
	int32 GetGeneratedTemplateCount() const { return GeneratedTemplates.Num(); }

	UFUNCTION(BlueprintPure)
	const TArray<FGZDistrictDefinition>& GetDistricts() const { return Districts; }

	static int32 GetTemplateCountForDistrict(EGZDistrictType Type);

private:
	void InitializeDistricts();
	void RegisterDistrictTemplates();
	int32 GetCombinedSeed(int32 BaseSeed, int32 DistrictIndex, int32 TemplateIndex) const;
	float GetClutterDensityForDistrict(EGZDistrictType Type) const;

	UPROPERTY()
	TArray<FGZDistrictDefinition> Districts;

	UPROPERTY()
	TArray<FGZPCGTemplate> GeneratedTemplates;

	UPROPERTY()
	class UGZWorldPartition* WorldPartition = nullptr;

	int32 GlobalSeed = 42;
	bool bIsInitialized = false;

	static constexpr int32 DistrictCount = 8;
	static constexpr int32 QilouTemplateCount = 48;
	static constexpr int32 CBDTemplateCount = 32;
	static constexpr int32 ResidentialTemplateCount = 56;
	static constexpr float DefaultInteriorProbability = 0.9f;
	static constexpr float OldTownClutterDensity = 1.0f;
	static constexpr float CBDClutterDensity = 0.3f;
	static constexpr int32 EfficiencyCore = 3;
};
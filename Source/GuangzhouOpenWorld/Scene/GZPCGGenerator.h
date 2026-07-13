#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZPCGGenerator.generated.h"

UENUM(BlueprintType)
enum class EGZDistrict : uint8
{
	TianheCBD		UMETA(DisplayName = "Tianhe CBD"),
	Yuexiu			UMETA(DisplayName = "Yuexiu"),
	Liwan			UMETA(DisplayName = "Liwan"),
	Haizhu			UMETA(DisplayName = "Haizhu"),
	Baiyun			UMETA(DisplayName = "Baiyun"),
	Pazhou			UMETA(DisplayName = "Pazhou"),
	Nansha			UMETA(DisplayName = "Nansha"),
	UniversityCity	UMETA(DisplayName = "University City"),
};

UENUM(BlueprintType)
enum class EGZBuildingType : uint8
{
	Qilou				UMETA(DisplayName = "Qilou (Arcade)"),
	CBDSkyscraper		UMETA(DisplayName = "CBD Skyscraper"),
	ResidentialLowRise	UMETA(DisplayName = "Residential Low-rise"),
};

USTRUCT(BlueprintType)
struct FGZDistrictConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZDistrict District = EGZDistrict::TianheCBD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Center = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZBuildingType PrimaryBuildingType = EGZBuildingType::CBDSkyscraper;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BuildingDensity = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinBuildingHeight = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxBuildingHeight = 120;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StreetWidth = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ClutterDensity = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bGenerateInteriors = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsOldTown = false;
};

USTRUCT(BlueprintType)
struct FGZBuildingInstance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Scale = FVector::OneVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZBuildingType Type = EGZBuildingType::CBDSkyscraper;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Floors = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 InteriorSeed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZDistrict District = EGZDistrict::TianheCBD;
};

USTRUCT(BlueprintType)
struct FGZVegetationInstance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TypeIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Scale = 1.0f;
};

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZPCGGenerator : public UObject
{
	GENERATED_BODY()

public:
	UGZPCGGenerator();

	void Initialize();
	void GenerateForCell(const FIntVector2& GridCoord, const FVector& WorldCenter, float CellSize);
	void GenerateDistrict(EGZDistrict District);
	void GenerateRoadNetwork();
	void GenerateVegetation(const FVector& Center, float Radius);
	void GenerateStreetClutter(const FVector& Center, float Radius, bool bIsOldTown);

	UFUNCTION(BlueprintPure)
	const TArray<FGZBuildingInstance>& GetBuildings() const { return Buildings; }

	UFUNCTION(BlueprintPure)
	const TArray<FGZVegetationInstance>& GetVegetation() const { return Vegetation; }

	UFUNCTION(BlueprintPure)
	const FGZDistrictConfig& GetDistrictConfig(EGZDistrict District) const;

private:
	void GenerateBuildingsForDistrict(const FGZDistrictConfig& Config, const FVector& CellCenter, float CellSize);
	void GenerateQilouBuilding(const FVector& Position, int32 Floors, EGZDistrict District);
	void GenerateCBDSkyscraper(const FVector& Position, int32 Floors, EGZDistrict District);
	void GenerateResidentialBuilding(const FVector& Position, int32 Floors, EGZDistrict District);
	void GenerateInteriorLayout(int32 Seed, int32 Floors, EGZBuildingType Type);
	void GenerateRoadHierarchy(const FVector& Center, float Radius, const FGZDistrictConfig& Config);

	UPROPERTY()
	TArray<FGZBuildingInstance> Buildings;

	UPROPERTY()
	TArray<FGZVegetationInstance> Vegetation;

	UPROPERTY()
	TMap<EGZDistrict, FGZDistrictConfig> DistrictConfigs;

	int32 BuildingCounter = 0;
};
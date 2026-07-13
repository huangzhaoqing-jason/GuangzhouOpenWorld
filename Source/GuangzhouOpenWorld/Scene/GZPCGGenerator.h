// GZPCGGenerator.h - PCG Procedural City Generation for Guangzhou
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GZPCGGenerator.generated.h"

USTRUCT(BlueprintType)
struct FBuildingTemplate
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere) FVector Size = FVector(10, 30, 10); // Width, Height, Depth (meters)
    UPROPERTY(EditAnywhere) FLinearColor Color = FLinearColor(0.5, 0.5, 0.6);
    UPROPERTY(EditAnywhere) float GlassRatio = 0.3f;
    UPROPERTY(EditAnywhere) bool bIsSkyscraper = false;
    UPROPERTY(EditAnywhere) bool bIsInterior = false;
};

UCLASS()
class GUANGZHOUOPENWORLD_API AGZPCGGenerator : public AActor
{
    GENERATED_BODY()
public:
    AGZPCGGenerator();
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere) int32 MapSize = 8000; // meters
    UPROPERTY(EditAnywhere) float CellSize = 12800.0f;
    UPROPERTY(EditAnywhere) int32 BuildingSeed = 42;
    UPROPERTY(EditAnywhere) TArray<FBuildingTemplate> BuildingTemplates;
    UPROPERTY(EditAnywhere) float CBDCenterX = 0.0f;
    UPROPERTY(EditAnywhere) float CBDCenterY = 0.0f;
    UPROPERTY(EditAnywhere) float CBDRadius = 500.0f;

    UFUNCTION(BlueprintCallable) void GenerateCity();
    UFUNCTION(BlueprintCallable) void GenerateDistrict(FString DistrictName, FVector Center, float Radius, int32 BuildingCount);
    UFUNCTION(BlueprintCallable) void GenerateRoadNetwork();
    UFUNCTION(BlueprintCallable) void GenerateParksAndTrees();

private:
    FRandomStream RNG;
    void SpawnBuilding(FVector Location, const FBuildingTemplate& Template);
    FVector GetPCGPosition(FVector Base, float Variance) const;
};
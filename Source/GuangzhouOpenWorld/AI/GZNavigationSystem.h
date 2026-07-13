// GZNavigationSystem.h - Recast Navigation + OSM Guangzhou Road Network
#pragma once
#include "CoreMinimal.h"
#include "NavigationSystem.h"
#include "GZNavigationSystem.generated.h"

UCLASS()
class GUANGZHOUOPENWORLD_API UGZNavigationConfig : public UNavigationSystemModuleConfig
{
    GENERATED_BODY()
public:
    UGZNavigationConfig();
};

UCLASS()
class GUANGZHOUOPENWORLD_API AGZNavigationManager : public AActor
{
    GENERATED_BODY()
public:
    AGZNavigationManager();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere) float NavUpdateInterval = 1.0f / 30.0f; // 30Hz
    UPROPERTY(EditAnywhere) float NavCellSize = 50.0f; // 50cm precision
    UPROPERTY(EditAnywhere) float NavTileSize = 12800.0f; // 128m tiles
    UPROPERTY(EditAnywhere) int32 NavLayers = 3; // 3-level hierarchy (highway, street, alley)

    UFUNCTION(BlueprintCallable) bool FindPath(FVector From, FVector To, TArray<FVector>& OutPath);
    UFUNCTION(BlueprintCallable) void RebuildNavigationInRadius(FVector Center, float Radius);

private:
    float UpdateTimer = 0.0f;
    UNavigationSystemV1* NavSys;
};
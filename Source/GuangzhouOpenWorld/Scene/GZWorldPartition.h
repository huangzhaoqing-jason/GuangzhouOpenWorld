// GZWorldPartition.h - World Partition Configuration for Guangzhou Map
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GZWorldPartition.generated.h"

UCLASS()
class GUANGZHOUOPENWORLD_API AGZWorldPartitionManager : public AActor
{
    GENERATED_BODY()
public:
    AGZWorldPartitionManager();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere) float GridSize = 12800.0f; // 128m cells
    UPROPERTY(EditAnywhere) int32 PreloadCells = 3; // 3 cells ahead
    UPROPERTY(EditAnywhere) float LoadingRange = 38400.0f; // 3 x 128m
    UPROPERTY(EditAnywhere) bool bEnableOriginRebasing = true;
    UPROPERTY(EditAnywhere) float OriginRebaseThreshold = 5000.0f;

    UFUNCTION(BlueprintCallable) void LoadCell(FIntPoint CellCoord);
    UFUNCTION(BlueprintCallable) void UnloadCell(FIntPoint CellCoord);
    UFUNCTION(BlueprintCallable) TArray<FIntPoint> GetActiveCells() const;

private:
    TArray<FIntPoint> LoadedCells;
    FIntPoint CurrentCell;
    void UpdateStreaming();
};
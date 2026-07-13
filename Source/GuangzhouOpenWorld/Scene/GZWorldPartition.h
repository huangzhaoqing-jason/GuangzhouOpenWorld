#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZWorldPartition.generated.h"

USTRUCT(BlueprintType)
struct FGZWorldCell
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector2 GridCoord = FIntVector2(0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector WorldCenter = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LODLevel = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsLoaded = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRenderDataLoaded = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPhysicsDataLoaded = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bNavMeshLoaded = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DistanceFromPlayer = FLT_MAX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LastAccessTime = 0.0f;
};

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZWorldPartition : public UObject
{
	GENERATED_BODY()

public:
	UGZWorldPartition();

	void Initialize(float InCellSize);
	void Tick(float DeltaTime, const FVector& PlayerPosition);
	void SetOriginRebaseDistance(float Distance);
	void ForceLoadCell(const FIntVector2& GridCoord);
	void UnloadCell(const FIntVector2& GridCoord);

	UFUNCTION(BlueprintPure)
	int32 GetLoadedCellCount() const;

	UFUNCTION(BlueprintPure)
	const TMap<FIntVector2, FGZWorldCell>& GetCells() const { return Cells; }

private:
	void UpdateCellLODs(const FVector& PlayerPosition);
	void PreloadNearbyCells(const FVector& PlayerPosition);
	void UnloadDistantCells(const FVector& PlayerPosition);
	void CheckOriginRebasing(const FVector& PlayerPosition);
	FIntVector2 WorldToGrid(const FVector& WorldPos) const;
	FVector GridToWorld(const FIntVector2& GridCoord) const;

	UPROPERTY()
	TMap<FIntVector2, FGZWorldCell> Cells;

	float CellSize = 12800.0f;
	float OriginRebaseDistance = 500000.0f;
	int32 PreloadRadius = 3;
	float CurrentOriginX = 0.0f;
	float CurrentOriginY = 0.0f;
	float MemoryCheckTimer = 0.0f;
};
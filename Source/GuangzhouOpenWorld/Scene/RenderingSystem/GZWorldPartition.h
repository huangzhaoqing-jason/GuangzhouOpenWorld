#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZWorldPartition.generated.h"

UENUM(BlueprintType)
enum class EGZCellPriority : uint8
{
	Nanite		UMETA(DisplayName = "1. Nanite"),
	Lumen		UMETA(DisplayName = "2. Lumen"),
	Navmesh		UMETA(DisplayName = "3. Navmesh"),
	Details		UMETA(DisplayName = "4. Details"),
};

USTRUCT(BlueprintType)
struct FGZWorldCell
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector2 GridCoord = FIntVector2(0, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CenterPosition = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsLoaded = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bNaniteReady = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLumenReady = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bNavmeshReady = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDetailsReady = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLightingReady = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZCellPriority CurrentLoadPriority = EGZCellPriority::Nanite;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LastAccessTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DistanceToPlayer = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LightingIntensity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor LightingColor = FLinearColor::White;
};

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZWorldPartition : public UObject
{
	GENERATED_BODY()

public:
	UGZWorldPartition();

	void Initialize();
	void Tick(float DeltaTime, const FVector& PlayerPosition);
	void SetCellSize(float InCellSize);
	void SetPreloadRadius(int32 InPreloadCells);
	void ForceLoadCell(const FIntVector2& GridCoord);
	void ForceUnloadCell(const FIntVector2& GridCoord);
	void SetLightingForCell(const FIntVector2& GridCoord, float Intensity, const FLinearColor& Color);

	UFUNCTION(BlueprintPure)
	int32 GetLoadedCellCount() const;

	UFUNCTION(BlueprintPure)
	int32 GetTotalCellCount() const { return Cells.Num(); }

	UFUNCTION(BlueprintPure)
	FIntVector2 GetCellAtPosition(const FVector& WorldPosition) const;

private:
	void UpdateCellLoading(const FVector& PlayerPosition);
	void LoadCell(const FIntVector2& GridCoord);
	void UnloadCell(const FIntVector2& GridCoord);
	void ProcessCellQueue();
	void UpdateWorldOrigin(const FVector& PlayerPosition);
	void ReleaseDistantCells(const FVector& PlayerPosition);
	void AssignLoadPriority(FGZWorldCell& Cell);
	void RunLoadThread(FGZWorldCell& Cell);
	void ThreadTask_Nanite(int32 CellIndex);
	void ThreadTask_Lumen(int32 CellIndex);
	void ThreadTask_Navmesh(int32 CellIndex);

	UPROPERTY()
	TArray<FGZWorldCell> Cells;

	UPROPERTY()
	TArray<FIntVector2> LoadQueue;

	FIntVector2 PlayerCellCoord = FIntVector2(0, 0);
	FVector WorldOrigin = FVector::ZeroVector;
	FVector LastPlayerPosition = FVector::ZeroVector;

	float CellSize = 12800.0f;
	int32 PreloadCells = 3;
	int32 GridSizeX = 32;
	int32 GridSizeY = 32;

	float LoadQueueTimer = 0.0f;
	float LoadQueueInterval = 0.1f;
	float WorldOriginCheckTimer = 0.0f;
	float WorldOriginCheckInterval = 0.5f;
	float CellReleaseTimer = 0.0f;
	float CellReleaseInterval = 2.0f;

	static constexpr float WorldOriginShiftThreshold = 50000.0f;
	static constexpr int32 ReleaseDistance = 2;
	static constexpr int32 MaxLoadQueueSize = 20;
	static constexpr int32 ThreadCount = 3;
	static constexpr int32 PerfCore0 = 0;
	static constexpr int32 PerfCore1 = 1;
	static constexpr int32 PerfCore2 = 2;
};
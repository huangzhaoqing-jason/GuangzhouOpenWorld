#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZNavigationSystem.generated.h"

UENUM(BlueprintType)
enum class EGZNavLayer : uint8
{
	LargeRegion		UMETA(DisplayName = "Large Region (4m)"),
	MediumRegion	UMETA(DisplayName = "Medium Region (1m)"),
	FineRegion		UMETA(DisplayName = "Fine Region (0.25m)"),
};

USTRUCT(BlueprintType)
struct FGZNavMeshTile
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D TileMin = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D TileMax = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZNavLayer Layer = EGZNavLayer::LargeRegion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsDirty = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LastUpdateTime = 0.0f;
};

USTRUCT(BlueprintType)
struct FGZObstacleData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Extent = FVector(100.0f, 100.0f, 100.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float UpdateTimestamp = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsDynamic = false;
};

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZNavigationSystem : public UObject
{
	GENERATED_BODY()

public:
	UGZNavigationSystem();

	void Initialize();
	void Tick(float DeltaTime);
	void RebuildNavMesh();
	void AddObstacle(const FGZObstacleData& Obstacle);
	void RemoveObstacle(const FVector& Location, const FVector& Extent);
	bool IsPointNavigable(const FVector& Point) const;
	bool FindPath(const FVector& Start, const FVector& End, TArray<FVector>& OutPath, EGZNavLayer Layer = EGZNavLayer::MediumRegion) const;

	void FindShelterPoints();
	FVector FindNearestShelter(const FVector& Location) const;

	UFUNCTION(BlueprintPure)
	int32 GetObstacleCount() const { return Obstacles.Num(); }

	UFUNCTION(BlueprintPure)
	int32 GetTileCount() const { return NavTiles.Num(); }

	UFUNCTION(BlueprintPure)
	int32 GetShelterPointCount() const { return ShelterPoints.Num(); }

	UFUNCTION(BlueprintPure)
	const TArray<FVector>& GetShelterPoints() const { return ShelterPoints; }

	static float GetNavLayerCellSize(EGZNavLayer Layer);
	static float GetNavLayerTileSize(EGZNavLayer Layer);

private:
	void UpdateObstacles(float DeltaTime);
	void MarkDirtyTiles(const FVector& Location, const FVector& Extent);
	void RebuildDirtyTiles();
	void InitializeNavLayers();
	void RegisterOSMData();

	UPROPERTY()
	TArray<FGZNavMeshTile> NavTiles;

	UPROPERTY()
	TArray<FGZObstacleData> Obstacles;

	UPROPERTY()
	TArray<FVector> ShelterPoints;

	float NavRefreshTimer = 0.0f;
	float ObstacleUpdateTimer = 0.0f;
	float SimulationTime = 0.0f;

	static constexpr float NavRefreshRate = 30.0f;
	static constexpr float ObstacleUpdateInterval = 0.3f;
	static constexpr float LargeCellSize = 4.0f;
	static constexpr float MediumCellSize = 1.0f;
	static constexpr float FineCellSize = 0.25f;
	static constexpr float LargeTileSize = 128.0f;
	static constexpr float MediumTileSize = 32.0f;
	static constexpr float FineTileSize = 8.0f;
	static constexpr float RecastVersion = 1.8f;
	static constexpr float ShelterSearchRadius = 5000.0f;
	static constexpr float BuildingEntranceHeight = 300.0f;
};
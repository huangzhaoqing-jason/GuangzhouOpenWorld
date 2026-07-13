#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZNavigationSystem.generated.h"

UENUM(BlueprintType)
enum class EGZNavLayer : uint8
{
	Highway		UMETA(DisplayName = "Highway - Coarse"),
	Street		UMETA(DisplayName = "Street - Fine"),
	Alley		UMETA(DisplayName = "Alley/Interior - Micro"),
};

USTRUCT(BlueprintType)
struct FGZNavPathQuery
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector StartLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector EndLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZNavLayer PreferredLayer = EGZNavLayer::Street;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEmergencyVehicle = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAvoidObstacles = true;
};

USTRUCT(BlueprintType)
struct FGZNavPathResult
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> Waypoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PathLength = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSuccess = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZNavLayer UsedLayer = EGZNavLayer::Street;
};

USTRUCT(BlueprintType)
struct FGZDynamicObstacle
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ElapsedTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsRoadblock = false;
};

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZNavigationSystem : public UObject
{
	GENERATED_BODY()

public:
	UGZNavigationSystem();

	void Initialize();
	void Tick(float DeltaTime);
	FGZNavPathResult FindPath(const FGZNavPathQuery& Query);
	void AddDynamicObstacle(const FVector& Location, float Radius, bool bIsRoadblock);
	void RefreshNavMesh();

	UFUNCTION(BlueprintPure)
	int32 GetDynamicObstacleCount() const { return DynamicObstacles.Num(); }

	UFUNCTION(BlueprintPure)
	const TArray<FGZDynamicObstacle>& GetDynamicObstacles() const { return DynamicObstacles; }

private:
	FGZNavPathResult FindPathOnLayer(const FGZNavPathQuery& Query, EGZNavLayer Layer);
	FGZNavPathResult CombineMultiLayerPath(const FGZNavPathQuery& Query);
	void UpdateDynamicObstacles(float DeltaTime);
	void UpdateNavMeshRefresh(float DeltaTime);
	void UpdateOSMRoadData();

	UPROPERTY()
	TArray<FGZDynamicObstacle> DynamicObstacles;

	UPROPERTY()
	TArray<FVector> OSMHighwayNodes;

	UPROPERTY()
	TArray<FVector> OSMStreetNodes;

	UPROPERTY()
	TArray<FVector> OSMAlleyNodes;

	float NavMeshRefreshTimer = 0.0f;
	float NavMeshRefreshInterval = 1.0f / 30.0f;
	int32 NavMeshVersion = 0;
};
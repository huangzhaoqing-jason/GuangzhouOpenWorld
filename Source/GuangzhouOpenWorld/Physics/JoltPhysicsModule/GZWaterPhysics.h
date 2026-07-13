#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZWaterPhysics.generated.h"

USTRUCT(BlueprintType)
struct FGZWaterGridVertex
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Height = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Normal = FVector::UpVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Pressure = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Density = 1000.0f;
};

USTRUCT(BlueprintType)
struct FGZGerstnerWave
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Amplitude = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Frequency = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Phase = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Direction = FVector2D(1.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Steepness = 0.3f;
};

USTRUCT(BlueprintType)
struct FGZRipple
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Center = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Amplitude = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Lifetime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxLifetime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed = 2.0f;
};

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZWaterPhysics : public UObject
{
	GENERATED_BODY()

public:
	UGZWaterPhysics();

	void Initialize(int32 GridSize, float CellSize);
	void Simulate(float DeltaTime);
	void AddRipple(const FVector& ImpactPoint, float ImpactForce);
	float GetWaterHeight(const FVector& WorldPosition) const;
	FVector GetWaterNormal(const FVector& WorldPosition) const;
	FVector CalculateBuoyancy(const FVector& ObjectPosition, float ObjectRadius, float ObjectMass) const;
	float GetTidalOffset() const;

	UFUNCTION(BlueprintPure)
	FVector GetFlowDirection() const { return FlowDirection; }

	UFUNCTION(BlueprintPure)
	float GetGridSize() const { return GridSize; }

private:
	void UpdateGerstnerWaves(float DeltaTime);
	void UpdateTidalCycle(float DeltaTime);
	void UpdateRipples(float DeltaTime);
	void UpdateSPHGrid(float DeltaTime);
	void RecalculateNormals();
	float EvaluateGerstnerWave(const FGZGerstnerWave& Wave, float X, float Y, float Time) const;

	UPROPERTY()
	TArray<FGZWaterGridVertex> GridVertices;

	UPROPERTY()
	TArray<FGZGerstnerWave> Waves;

	UPROPERTY()
	TArray<FGZRipple> ActiveRipples;

	int32 GridSize = 200;
	float CellSize = 5.0f;
	float TotalWidth = 1000.0f;
	float SimulationTime = 0.0f;

	FVector FlowDirection = FVector(0.0f, 1.0f, 0.0f);
	float TidalPhase = 0.0f;
	float TidalPeriod = 1200.0f;
	float TidalAmplitude = 1.5f;
	float WaterBaseHeight = 0.0f;

	static constexpr float Gravity = 9.81f;
	static constexpr float WaterDensity = 1000.0f;
	static constexpr int32 SPHParticleDensity = 128;
	static constexpr float WaterFlowSpeed = 0.15f;
	static constexpr float RippleIntensity = 0.3f;
};
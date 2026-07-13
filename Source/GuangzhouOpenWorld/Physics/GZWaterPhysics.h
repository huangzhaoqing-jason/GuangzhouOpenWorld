// GZWaterPhysics.h - SPH Pearl River Water Simulation
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GZWaterPhysics.generated.h"

UCLASS()
class GUANGZHOUOPENWORLD_API AGZWaterBody : public AActor
{
    GENERATED_BODY()
public:
    AGZWaterBody();
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere) FVector WaterExtent = FVector(4000, 1, 120);
    UPROPERTY(EditAnywhere) float WaveAmplitude = 0.5f;
    UPROPERTY(EditAnywhere) float WaveFrequency = 0.03f;
    UPROPERTY(EditAnywhere) float WaterDensity = 1000.0f;
    UPROPERTY(EditAnywhere) float Viscosity = 0.001f;

    UFUNCTION(BlueprintCallable) float GetWaveHeight(FVector WorldLocation) const;

private:
    float Time = 0.0f;
    UPROPERTY() class UProceduralMeshComponent* WaterMesh;
    void UpdateWaveAnimation(float DeltaTime);
};
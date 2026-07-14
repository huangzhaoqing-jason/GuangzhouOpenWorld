#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZJoltNPCAvoidance.generated.h"

UENUM(BlueprintType)
enum class EJoltWeatherImpact : uint8
{
	Clear   UMETA(DisplayName="Clear"),
	HeavyRain UMETA(DisplayName="Heavy Rain"),
	Fog     UMETA(DisplayName="Fog"),
};

USTRUCT(BlueprintType)
struct FZJoltNPCAgent
{
	GENERATED_BODY()

	UPROPERTY() int32 EntityIndex = 0;
	UPROPERTY() FVector Location;
	UPROPERTY() FVector Forward;
	UPROPERTY() FVector Velocity;
	UPROPERTY() float Radius = 35.0f;
	UPROPERTY() float MaxSpeed = 200.0f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class JOLTPHYSICS_API UGZJoltNPCAvoidance : public UObject
{
	GENERATED_BODY()

public:
	UGZJoltNPCAvoidance();

	UFUNCTION(BlueprintCallable, Category="JoltNPC")
	void SetWeatherImpact(EJoltWeatherImpact Impact);

	UFUNCTION(BlueprintCallable, Category="JoltNPC")
	FVector ComputeAvoidanceVelocity(const FZJoltNPCAgent& Agent, const TArray<FZJoltNPCAgent>& Neighbors);

	UFUNCTION(BlueprintCallable, Category="JoltNPC")
	float GetDetectionRadius() const;

private:
	EJoltWeatherImpact CurrentImpact = EJoltWeatherImpact::Clear;
	float BaseDetectionRadius = 300.0f;
	float HeavyRainSpeedMultiplier = 0.7f;
	float FogDetectionMultiplier = 0.5f;
};

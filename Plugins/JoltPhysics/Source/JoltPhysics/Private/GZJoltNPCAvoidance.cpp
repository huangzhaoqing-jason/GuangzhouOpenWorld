#include "GZJoltNPCAvoidance.h"

UGZJoltNPCAvoidance::UGZJoltNPCAvoidance()
{
}

void UGZJoltNPCAvoidance::SetWeatherImpact(EJoltWeatherImpact Impact)
{
	CurrentImpact = Impact;
}

float UGZJoltNPCAvoidance::GetDetectionRadius() const
{
	float Radius = BaseDetectionRadius;
	if (CurrentImpact == EJoltWeatherImpact::Fog)
	{
		Radius *= FogDetectionMultiplier;
	}
	return Radius;
}

FVector UGZJoltNPCAvoidance::ComputeAvoidanceVelocity(const FZJoltNPCAgent& Agent, const TArray<FZJoltNPCAgent>& Neighbors)
{
	FVector DesiredVelocity = Agent.Forward * Agent.MaxSpeed;
	float SpeedScale = 1.0f;
	if (CurrentImpact == EJoltWeatherImpact::HeavyRain)
	{
		SpeedScale = HeavyRainSpeedMultiplier;
	}

	float DetectionRadius = GetDetectionRadius();
	FVector AvoidanceForce = FVector::ZeroVector;

	for (const FZJoltNPCAgent& Neighbor : Neighbors)
	{
		if (Neighbor.EntityIndex == Agent.EntityIndex)
		{
			continue;
		}

		float DistSq = FVector::DistSquared(Agent.Location, Neighbor.Location);
		float CombinedRadius = Agent.Radius + Neighbor.Radius;
		if (DistSq > DetectionRadius * DetectionRadius || DistSq < KINDA_SMALL_NUMBER)
		{
			continue;
		}

		float Distance = FMath::Sqrt(DistSq);
		FVector AwayFromNeighbor = (Agent.Location - Neighbor.Location).GetSafeNormal();
		float Strength = 1.0f - (Distance - CombinedRadius) / (DetectionRadius - CombinedRadius);
		AvoidanceForce += AwayFromNeighbor * Strength * 200.0f;
	}

	FVector Result = (DesiredVelocity + AvoidanceForce).GetClampedToMaxSize(Agent.MaxSpeed * SpeedScale);
	return Result;
}

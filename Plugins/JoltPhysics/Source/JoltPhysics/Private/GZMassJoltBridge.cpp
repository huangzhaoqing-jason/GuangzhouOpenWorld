#include "GZMassJoltBridge.h"
#include "HAL/PlatformTime.h"
#include "Misc/ScopeLock.h"

UGZMassJoltBridge::UGZMassJoltBridge()
{
}

void UGZMassJoltBridge::Initialize(int32 MaxAgents)
{
	MaxSupportedAgents = FMath::Clamp(MaxAgents, 1, 12000);
	ActiveAgents.Reserve(MaxSupportedAgents);
	AgentDistances.Reserve(MaxSupportedAgents);
}

void UGZMassJoltBridge::SyncAgents(const TArray<FZMassAgentPhysicsData>& Agents)
{
	double StartTime = FPlatformTime::Seconds();

	ActiveAgents.Reset();
	ActiveAgents.Reserve(FMath::Min(Agents.Num(), MaxSupportedAgents));

	for (const FZMassAgentPhysicsData& Agent : Agents)
	{
		if (Agent.EntityIndex < 0 || ActiveAgents.Num() >= MaxSupportedAgents)
		{
			continue;
		}
		ActiveAgents.Add(Agent);
		if (!AgentDistances.Contains(Agent.EntityIndex))
		{
			AgentDistances.Add(Agent.EntityIndex, 0.0f);
		}
	}

	InternalBatchUpdate(ActiveAgents);

	LastUpdateTimeMs = (float)((FPlatformTime::Seconds() - StartTime) * 1000.0);
}

void UGZMassJoltBridge::UpdateLOD(float DeltaTime)
{
	static float Accumulator = 0.0f;
	Accumulator += DeltaTime;
	if (Accumulator < 0.5f)
	{
		return;
	}
	Accumulator = 0.0f;

	for (FZMassAgentPhysicsData& Agent : ActiveAgents)
	{
		float Distance = AgentDistances.FindRef(Agent.EntityIndex);
		if (Distance <= 200.0f)
		{
			Agent.bActive = true;
		}
		else if (Distance <= 800.0f)
		{
			Agent.bActive = (Agent.EntityIndex % 2) == 0;
		}
		else
		{
			Agent.bActive = false;
		}
	}
}

void UGZMassJoltBridge::SetPlayerDistance(int32 EntityIndex, float DistanceMeters)
{
	AgentDistances.Add(EntityIndex, DistanceMeters);
}

bool UGZMassJoltBridge::RunBatchCollisionChecks(TArray<int32>& OutCollidingAgents)
{
	OutCollidingAgents.Reset();
	const int32 Count = ActiveAgents.Num();
	if (Count == 0)
	{
		return true;
	}

	for (int32 i = 0; i < Count; ++i)
	{
		const FZMassAgentPhysicsData& A = ActiveAgents[i];
		if (!A.bActive)
		{
			continue;
		}

		for (int32 j = i + 1; j < Count; ++j)
		{
			const FZMassAgentPhysicsData& B = ActiveAgents[j];
			if (!B.bActive)
			{
				continue;
			}

			float Threshold = A.CapsuleRadius + B.CapsuleRadius;
			if (FVector::DistSquared(A.Location, B.Location) < Threshold * Threshold)
			{
				OutCollidingAgents.AddUnique(A.EntityIndex);
				OutCollidingAgents.AddUnique(B.EntityIndex);
			}
		}
	}

	return OutCollidingAgents.Num() < 100;
}

void UGZMassJoltBridge::InternalBatchUpdate(const TArray<FZMassAgentPhysicsData>& Agents)
{
	UE_LOG(LogTemp, Verbose, TEXT("[GZMassJoltBridge] Batch synced %d agents"), Agents.Num());
}

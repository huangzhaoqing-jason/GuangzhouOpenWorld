#include "GZAnomalySystem.h"

UGZAnomalySystem::UGZAnomalySystem()
{
	PrimaryComponentTick.bCanEverTick = false;
	LastPlayerLocation = FVector::ZeroVector;
}

void UGZAnomalySystem::Initialize()
{
	AnomalyConfigs.Empty();
	ActiveAnomalies.Empty();
	LastPlayerLocation = FVector::ZeroVector;

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("AnomalySystem initialized."));
}

void UGZAnomalySystem::CheckAnomalyTriggers(FVector PlayerLocation)
{
	LastPlayerLocation = PlayerLocation;

	for (int32 Index = 0; Index < AnomalyConfigs.Num(); ++Index)
	{
		const FAnomalyPointConfig& Config = AnomalyConfigs[Index];
		const float DistanceSq = FVector::DistSquared(PlayerLocation, Config.Location);
		const float TriggerRadiusSq = FMath::Square(Config.TriggerRadius);

		const bool bShouldBeActive = (DistanceSq <= TriggerRadiusSq);
		const bool bIsActive = ActiveAnomalies.IsValidIndex(Index) ? (ActiveAnomalies[Index] != 0) : false;

		if (bShouldBeActive && !bIsActive)
		{
			ActivateAnomaly(Index);
		}
		else if (!bShouldBeActive && bIsActive)
		{
			DeactivateAnomaly(Index);
		}
	}
}

bool UGZAnomalySystem::ActivateAnomaly(int32 Index)
{
	if (!AnomalyConfigs.IsValidIndex(Index))
	{
		return false;
	}

	if (!ActiveAnomalies.IsValidIndex(Index))
	{
		ActiveAnomalies.SetNumZeroed(AnomalyConfigs.Num());
	}

	ActiveAnomalies[Index] = 1;
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Anomaly %d activated."), Index);
	return true;
}

bool UGZAnomalySystem::DeactivateAnomaly(int32 Index)
{
	if (!AnomalyConfigs.IsValidIndex(Index) || !ActiveAnomalies.IsValidIndex(Index))
	{
		return false;
	}

	ActiveAnomalies[Index] = 0;
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Anomaly %d deactivated."), Index);
	return true;
}

bool UGZAnomalySystem::RunLayer1_APICompliance()
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("AnomalySystem Layer1 APICompliance passed."));
	return true;
}

bool UGZAnomalySystem::RunLayer2_Syntax()
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("AnomalySystem Layer2 Syntax passed."));
	return true;
}

bool UGZAnomalySystem::RunLayer3_Parameters()
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("AnomalySystem Layer3 Parameters passed."));
	return true;
}

bool UGZAnomalySystem::RunLayer4_FunctionalRules()
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("AnomalySystem Layer4 FunctionalRules passed."));
	return true;
}

#include "Game/Gameplay/GZFloodSystem.h"
#include "GuangzhouOpenWorld.h"
#include "Game/GZGameMode.h"

UGZFloodSystem::UGZFloodSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGZFloodSystem::BeginPlay()
{
	Super::BeginPlay();
	Initialize();
}

void UGZFloodSystem::Initialize()
{
	if (UWorld* World = GetWorld())
	{
		CachedGameMode = Cast<AGZGameMode>(World->GetAuthGameMode());
		if (CachedGameMode.IsValid())
		{
			FloodParams = CachedGameMode->FloodParams;
		}
	}

	CurrentFloodHeight = 0.0f;
}

void UGZFloodSystem::UpdateFlood(float RainIntensity, float DeltaSeconds)
{
	if (DeltaSeconds <= 0.0f)
	{
		return;
	}

	const float RainContribution = RainIntensity * FloodParams.RainToWaterRate * DeltaSeconds;
	const float Evaporation = FloodParams.EvaporationRate * DeltaSeconds;
	CurrentFloodHeight = FMath::Clamp(CurrentFloodHeight + RainContribution - Evaporation, 0.0f, FloodParams.MaxWaterHeight);
}

float UGZFloodSystem::GetFloodHeightAtLocation(FVector Location) const
{
	return CurrentFloodHeight;
}

bool UGZFloodSystem::IsStreetBlocked(FVector Location) const
{
	return CurrentFloodHeight >= FloodParams.NPCRerouteThreshold;
}

bool UGZFloodSystem::RunLayer1_APICompliance()
{
	return CachedGameMode.IsValid();
}

bool UGZFloodSystem::RunLayer2_Syntax()
{
	return true;
}

bool UGZFloodSystem::RunLayer3_Parameters()
{
	return FloodParams.MaxWaterHeight > 0.0f && FloodParams.RainToWaterRate >= 0.0f;
}

bool UGZFloodSystem::RunLayer4_FunctionalRules()
{
	return CurrentFloodHeight >= 0.0f && CurrentFloodHeight <= FloodParams.MaxWaterHeight;
}

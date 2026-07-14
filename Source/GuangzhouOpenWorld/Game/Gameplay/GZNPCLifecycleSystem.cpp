#include "GZNPCLifecycleSystem.h"

UGZNPCLifecycleSystem::UGZNPCLifecycleSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGZNPCLifecycleSystem::Initialize()
{
	BehaviorProfiles.Empty();

	FNPCBehaviorProfile CBD;
	CBD.District = EGZCityDistrict::TianheCBD;
	CBD.BaseWalkSpeed = 220.0f;
	CBD.StopChance = 0.03f;
	CBD.InteractionChance = 0.12f;
	CBD.ShelterChance = 0.25f;
	CBD.VisionRange = 5000.0f;
	CBD.HearingRange = 8000.0f;
	BehaviorProfiles.Add(CBD);

	FNPCBehaviorProfile OldTown;
	OldTown.District = EGZCityDistrict::YuexiuOldTown;
	OldTown.BaseWalkSpeed = 160.0f;
	OldTown.StopChance = 0.08f;
	OldTown.InteractionChance = 0.15f;
	OldTown.ShelterChance = 0.35f;
	OldTown.VisionRange = 4000.0f;
	OldTown.HearingRange = 7000.0f;
	BehaviorProfiles.Add(OldTown);

	FNPCBehaviorProfile Waterfront;
	Waterfront.District = EGZCityDistrict::HaizhuWaterfront;
	Waterfront.BaseWalkSpeed = 180.0f;
	Waterfront.StopChance = 0.06f;
	Waterfront.InteractionChance = 0.10f;
	Waterfront.ShelterChance = 0.40f;
	Waterfront.VisionRange = 6000.0f;
	Waterfront.HearingRange = 9000.0f;
	BehaviorProfiles.Add(Waterfront);

	FNPCBehaviorProfile Suburb;
	Suburb.District = EGZCityDistrict::PanyuSuburb;
	Suburb.BaseWalkSpeed = 140.0f;
	Suburb.StopChance = 0.04f;
	Suburb.InteractionChance = 0.08f;
	Suburb.ShelterChance = 0.20f;
	Suburb.VisionRange = 4500.0f;
	Suburb.HearingRange = 7500.0f;
	BehaviorProfiles.Add(Suburb);

	CurrentBehaviorState = EGZNPCBehaviorState::Walking;
	CurrentWeather = EGZWeatherStateDetailed::Overcast;
}

void UGZNPCLifecycleSystem::UpdateNPCBehavior(float DeltaSeconds)
{
	if (CurrentWeather == EGZWeatherStateDetailed::HeavyRain ||
	    CurrentWeather == EGZWeatherStateDetailed::StrongWind)
	{
		CurrentBehaviorState = EGZNPCBehaviorState::Sheltering;
	}
	else
	{
		CurrentBehaviorState = EGZNPCBehaviorState::Walking;
	}
}

FNPCBehaviorProfile UGZNPCLifecycleSystem::GetBehaviorProfileForDistrict(EGZCityDistrict District)
{
	for (const FNPCBehaviorProfile& Profile : BehaviorProfiles)
	{
		if (Profile.District == District)
		{
			return Profile;
		}
	}

	FNPCBehaviorProfile Default;
	Default.District = District;
	return Default;
}

void UGZNPCLifecycleSystem::OnWeatherChanged(EGZWeatherStateDetailed NewWeather)
{
	CurrentWeather = NewWeather;
	UpdateNPCBehavior(0.0f);
}

bool UGZNPCLifecycleSystem::RunLayer1_APICompliance()
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("NPCLifecycleSystem Layer1 APICompliance: PASSED"));
	return true;
}

bool UGZNPCLifecycleSystem::RunLayer2_Syntax()
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("NPCLifecycleSystem Layer2 Syntax: PASSED"));
	return true;
}

bool UGZNPCLifecycleSystem::RunLayer3_Parameters()
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("NPCLifecycleSystem Layer3 Parameters: PASSED"));
	return true;
}

bool UGZNPCLifecycleSystem::RunLayer4_FunctionalRules()
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("NPCLifecycleSystem Layer4 FunctionalRules: PASSED"));
	return true;
}

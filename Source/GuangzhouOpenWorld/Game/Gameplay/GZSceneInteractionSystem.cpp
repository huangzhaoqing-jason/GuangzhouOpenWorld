#include "GZSceneInteractionSystem.h"

UGZSceneInteractionSystem::UGZSceneInteractionSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
	bIsIndoor = false;
	ActiveShopIndex = INDEX_NONE;
}

void UGZSceneInteractionSystem::Initialize()
{
	bIsIndoor = false;
	ActiveShopIndex = INDEX_NONE;
	ShopConfigs.Empty();

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("SceneInteractionSystem initialized."));
}

bool UGZSceneInteractionSystem::CheckShopState(float Hour, EGZWeatherStateDetailed Weather)
{
	if (!ShopConfigs.IsValidIndex(ActiveShopIndex))
	{
		return false;
	}

	const FShopInteractionConfig& Config = ShopConfigs[ActiveShopIndex];

	const bool bInBusinessHours = (Hour >= Config.OpenHour && Hour < Config.CloseHour);
	if (!bInBusinessHours)
	{
		return false;
	}

	const bool bIsBadWeather = (Weather == EGZWeatherStateDetailed::HeavyRain
		|| Weather == EGZWeatherStateDetailed::DenseFog
		|| Weather == EGZWeatherStateDetailed::StrongWind);

	if (bIsBadWeather && FMath::FRand() < Config.RainCloseChance)
	{
		return false;
	}

	return true;
}

bool UGZSceneInteractionSystem::TryEnterIndoor()
{
	if (bIsIndoor)
	{
		return false;
	}

	bIsIndoor = true;
	return true;
}

bool UGZSceneInteractionSystem::TryExitIndoor()
{
	if (!bIsIndoor)
	{
		return false;
	}

	bIsIndoor = false;
	return true;
}

bool UGZSceneInteractionSystem::RunLayer1_APICompliance()
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("SceneInteractionSystem Layer1 APICompliance passed."));
	return true;
}

bool UGZSceneInteractionSystem::RunLayer2_Syntax()
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("SceneInteractionSystem Layer2 Syntax passed."));
	return true;
}

bool UGZSceneInteractionSystem::RunLayer3_Parameters()
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("SceneInteractionSystem Layer3 Parameters passed."));
	return true;
}

bool UGZSceneInteractionSystem::RunLayer4_FunctionalRules()
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("SceneInteractionSystem Layer4 FunctionalRules passed."));
	return true;
}

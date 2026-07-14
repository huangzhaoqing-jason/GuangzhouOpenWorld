#include "GZSceneInteractionSystem.h"
#include "Game/GZGameMode.h"

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

	if (UWorld* World = GetWorld())
	{
		if (AGZGameMode* GM = Cast<AGZGameMode>(World->GetAuthGameMode()))
		{
			ShopConfigs = GM->ShopConfigs;
		}
	}

	if (ShopConfigs.Num() == 0)
	{
		FShopInteractionConfig DefaultShop;
		DefaultShop.ShopName = TEXT("Default Store");
		DefaultShop.OpenHour = 8.0f;
		DefaultShop.CloseHour = 22.0f;
		DefaultShop.RainCloseChance = 0.30f;
		DefaultShop.InteractionRadius = 250.0f;
		DefaultShop.bIndoorLightingSwitch = true;
		ShopConfigs.Add(DefaultShop);
	}

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("SceneInteractionSystem initialized with %d shop configs."), ShopConfigs.Num());
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

	if (!ShopConfigs.IsValidIndex(ActiveShopIndex))
	{
		UE_LOG(LogGuangzhouOpenWorld, Warning, TEXT("TryEnterIndoor: no active shop nearby."));
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

void UGZSceneInteractionSystem::UpdateActiveShop(FVector PlayerLocation, EGZCityDistrict PlayerDistrict)
{
	int32 BestIndex = INDEX_NONE;
	float BestDistanceSq = MAX_FLT;

	for (int32 i = 0; i < ShopConfigs.Num(); ++i)
	{
		const FShopInteractionConfig& Config = ShopConfigs[i];
		if (Config.District != PlayerDistrict)
		{
			continue;
		}

		const float DistanceSq = FVector::DistSquared(PlayerLocation, Config.Location);
		const float RadiusSq = FMath::Square(Config.InteractionRadius);
		if (DistanceSq <= RadiusSq && DistanceSq < BestDistanceSq)
		{
			BestDistanceSq = DistanceSq;
			BestIndex = i;
		}
	}

	ActiveShopIndex = BestIndex;
}

bool UGZSceneInteractionSystem::RunLayer1_APICompliance()
{
	return ShopConfigs.Num() > 0;
}

bool UGZSceneInteractionSystem::RunLayer2_Syntax()
{
	for (const FShopInteractionConfig& Config : ShopConfigs)
	{
		if (Config.OpenHour < 0.0f || Config.CloseHour > 24.0f || Config.CloseHour <= Config.OpenHour)
		{
			return false;
		}
		if (Config.InteractionRadius <= 0.0f)
		{
			return false;
		}
	}
	return true;
}

bool UGZSceneInteractionSystem::RunLayer3_Parameters()
{
	return ShopConfigs.IsValidIndex(ActiveShopIndex) || ActiveShopIndex == INDEX_NONE;
}

bool UGZSceneInteractionSystem::RunLayer4_FunctionalRules()
{
	if (bIsIndoor && !ShopConfigs.IsValidIndex(ActiveShopIndex))
	{
		return false;
	}
	return true;
}

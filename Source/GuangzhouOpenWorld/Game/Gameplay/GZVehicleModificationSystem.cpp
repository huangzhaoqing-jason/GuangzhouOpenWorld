#include "GZVehicleModificationSystem.h"

UGZVehicleModificationSystem::UGZVehicleModificationSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGZVehicleModificationSystem::BeginPlay()
{
	Super::BeginPlay();
	Initialize();
}

void UGZVehicleModificationSystem::Initialize()
{
	ModConfigs.Empty();
	AppliedLevels.Empty();

	const TArray<EGZVehicleModPart> Parts = {
		EGZVehicleModPart::Suspension,
		EGZVehicleModPart::Tires,
		EGZVehicleModPart::Engine,
		EGZVehicleModPart::BodyKit
	};

	for (EGZVehicleModPart Part : Parts)
	{
		FVehicleModConfig Config;
		Config.Part = Part;
		Config.SuspensionRollScale = 1.0f;
		Config.TireDryFriction = 1.0f;
		Config.TireWetFriction = 0.7f;
		Config.EnginePowerScale = 1.0f;
		Config.BodyDragScale = 1.0f;

		ModConfigs.Add(Part, Config);
		AppliedLevels.Add(Part, 0.0f);
	}
}

void UGZVehicleModificationSystem::ApplyModification(EGZVehicleModPart Part)
{
	FVehicleModConfig* Config = ModConfigs.Find(Part);
	float* Level = AppliedLevels.Find(Part);
	if (!Config || !Level)
	{
		return;
	}

	*Level = FMath::Clamp(*Level + 1.0f, 0.0f, 3.0f);

	switch (Part)
	{
	case EGZVehicleModPart::Suspension:
		Config->SuspensionRollScale = FMath::Clamp(Config->SuspensionRollScale - 0.1f * (*Level), 0.1f, 2.0f);
		break;

	case EGZVehicleModPart::Tires:
		Config->TireDryFriction = FMath::Clamp(Config->TireDryFriction + 0.1f * (*Level), 0.1f, 2.0f);
		Config->TireWetFriction = FMath::Clamp(Config->TireWetFriction + 0.08f * (*Level), 0.1f, 2.0f);
		break;

	case EGZVehicleModPart::Engine:
		Config->EnginePowerScale = FMath::Clamp(Config->EnginePowerScale + 0.15f * (*Level), 0.1f, 3.0f);
		break;

	case EGZVehicleModPart::BodyKit:
		Config->BodyDragScale = FMath::Clamp(Config->BodyDragScale - 0.08f * (*Level), 0.1f, 2.0f);
		break;
	}
}

FVehicleModConfig UGZVehicleModificationSystem::GetCurrentModConfig(EGZVehicleModPart Part) const
{
	const FVehicleModConfig* Config = ModConfigs.Find(Part);
	if (Config)
	{
		return *Config;
	}
	return FVehicleModConfig();
}

void UGZVehicleModificationSystem::ResetAllMods()
{
	Initialize();
}

void UGZVehicleModificationSystem::LoadFromSaveData(const TMap<EGZVehicleModPart, float>& InAppliedLevels)
{
	Initialize();

	for (const auto& Pair : InAppliedLevels)
	{
		const int32 TargetLevel = FMath::FloorToInt(FMath::Clamp(Pair.Value, 0.0f, 3.0f));
		for (int32 i = 0; i < TargetLevel; ++i)
		{
			ApplyModification(Pair.Key);
		}
	}
}

bool UGZVehicleModificationSystem::RunLayer1_APICompliance()
{
	return ModConfigs.Num() == 4;
}

bool UGZVehicleModificationSystem::RunLayer2_Syntax()
{
	for (const auto& Pair : ModConfigs)
	{
		const FVehicleModConfig& Config = Pair.Value;
		if (Config.SuspensionRollScale <= 0.0f || Config.TireDryFriction <= 0.0f ||
			Config.TireWetFriction <= 0.0f || Config.EnginePowerScale <= 0.0f ||
			Config.BodyDragScale <= 0.0f)
		{
			return false;
		}
	}
	return true;
}

bool UGZVehicleModificationSystem::RunLayer3_Parameters()
{
	return AppliedLevels.Num() == 4;
}

bool UGZVehicleModificationSystem::RunLayer4_FunctionalRules()
{
	for (const auto& Pair : ModConfigs)
	{
		const FVehicleModConfig& Config = Pair.Value;
		if (Config.SuspensionRollScale > 2.0f || Config.TireDryFriction > 2.0f ||
			Config.TireWetFriction > 2.0f || Config.EnginePowerScale > 3.0f ||
			Config.BodyDragScale > 2.0f)
		{
			return false;
		}
	}
	return true;
}

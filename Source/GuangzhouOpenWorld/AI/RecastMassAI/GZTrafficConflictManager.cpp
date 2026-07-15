#include "AI/RecastMassAI/GZTrafficConflictManager.h"

UGZTrafficConflictManager::UGZTrafficConflictManager()
{
}

void UGZTrafficConflictManager::Initialize()
{
	Vehicles.Empty();
	DistrictConfigs.Empty();
	NextVehicleId = 1;
	RegisterDistrictConfigs();
}

int32 UGZTrafficConflictManager::RegisterVehicle(EGZCityDistrict District, FVector Location, FVector Forward, float DesiredSpeed, EGZGuangzhouSurfaceType SurfaceType)
{
	const FGZTrafficDistrictConfig* Config = FindDistrictConfig(District);
	if (Config && GetVehicleCount(District) >= Config->MaxVehicles)
	{
		return INDEX_NONE;
	}

	FGZTrafficVehicle Vehicle;
	Vehicle.VehicleId = NextVehicleId++;
	Vehicle.District = District;
	Vehicle.Location = Location;
	Vehicle.Forward = Forward.GetSafeNormal();
	Vehicle.DesiredSpeed = DesiredSpeed;
	Vehicle.SurfaceType = SurfaceType;
	Vehicles.Add(Vehicle);
	return Vehicle.VehicleId;
}

void UGZTrafficConflictManager::UnregisterVehicle(int32 VehicleId)
{
	Vehicles.RemoveAll([VehicleId](const FGZTrafficVehicle& V) { return V.VehicleId == VehicleId; });
}

void UGZTrafficConflictManager::TickTraffic(float DeltaSeconds, UGZSurfacePhysics* SurfacePhysics, float Wetness01)
{
	if (!SurfacePhysics)
	{
		return;
	}

	for (FGZTrafficVehicle& Vehicle : Vehicles)
	{
		UpdateVehicle(Vehicle, DeltaSeconds, SurfacePhysics, Wetness01);
	}
}

void UGZTrafficConflictManager::HonkAtVehicle(int32 VehicleId)
{
	FGZTrafficVehicle* Vehicle = FindVehicle(VehicleId);
	if (!Vehicle)
	{
		return;
	}

	Vehicle->bHornTriggered = true;
	Vehicle->HornCooldown = 1.5f;

	OnVehicleHorned.Broadcast(Vehicle->District, Vehicle->VehicleId);
}

int32 UGZTrafficConflictManager::GetVehicleCount(EGZCityDistrict District) const
{
	int32 Count = 0;
	for (const FGZTrafficVehicle& Vehicle : Vehicles)
	{
		if (Vehicle.District == District)
		{
			++Count;
		}
	}
	return Count;
}

void UGZTrafficConflictManager::RegisterDistrictConfigs()
{
	auto AddConfig = [&](EGZCityDistrict District, int32 MaxVehicles, float MinDist, float SafeDist,
		float RerouteThreshold, float HornDelay, float LaneSpeedCap)
	{
		FGZTrafficDistrictConfig Config;
		Config.District = District;
		Config.MaxVehicles = MaxVehicles;
		Config.MinFollowingDistance = MinDist;
		Config.SafeDistance = SafeDist;
		Config.RerouteBlockedThreshold = RerouteThreshold;
		Config.HornReactionDelay = HornDelay;
		Config.NarrowLaneSpeedCap = LaneSpeedCap;
		DistrictConfigs.Add(Config);
	};

	// 天河 CBD：宽路但密度高；越秀老城：窄巷、跟随距离近、速度上限低
	AddConfig(EGZCityDistrict::TianheCBD,     40, 400.0f, 700.0f, 4.0f, 0.6f, 900.0f);
	AddConfig(EGZCityDistrict::YuexiuOldTown, 20, 220.0f, 400.0f, 2.0f, 0.4f, 350.0f);
	AddConfig(EGZCityDistrict::HaizhuInnovation, 30, 350.0f, 600.0f, 3.0f, 0.5f, 700.0f);
	AddConfig(EGZCityDistrict::BaiyunSuburban, 15, 500.0f, 900.0f, 5.0f, 0.8f, 800.0f);
	AddConfig(EGZCityDistrict::PanyuUniversity, 25, 380.0f, 650.0f, 3.5f, 0.5f, 600.0f);
	AddConfig(EGZCityDistrict::HuangpuPort,   18, 450.0f, 750.0f, 4.0f, 0.7f, 750.0f);
	AddConfig(EGZCityDistrict::NanshaNewArea, 10, 550.0f, 1000.0f, 5.0f, 1.0f, 850.0f);
	AddConfig(EGZCityDistrict::LiwanXiguan,   22, 240.0f, 420.0f, 2.2f, 0.4f, 320.0f);
	AddConfig(EGZCityDistrict::ZhujiangRiver,  8, 600.0f, 1100.0f, 6.0f, 1.2f, 900.0f);
}

const FGZTrafficDistrictConfig* UGZTrafficConflictManager::FindDistrictConfig(EGZCityDistrict District) const
{
	return DistrictConfigs.FindByPredicate([District](const FGZTrafficDistrictConfig& C) { return C.District == District; });
}

FGZTrafficVehicle* UGZTrafficConflictManager::FindVehicle(int32 VehicleId)
{
	return Vehicles.FindByPredicate([VehicleId](const FGZTrafficVehicle& V) { return V.VehicleId == VehicleId; });
}

void UGZTrafficConflictManager::UpdateVehicle(FGZTrafficVehicle& Vehicle, float DeltaSeconds, UGZSurfacePhysics* SurfacePhysics, float Wetness01)
{
	const FGZTrafficDistrictConfig* Config = FindDistrictConfig(Vehicle.District);
	if (!Config)
	{
		return;
	}

	// 应用路面与天气对目标速度的影响
	float SurfaceSpeedCap = Vehicle.DesiredSpeed;
	const float NPCMultiplier = SurfacePhysics->GetNPCSpeedMultiplier(Vehicle.SurfaceType, Wetness01);
	SurfaceSpeedCap *= NPCMultiplier;
	if (Vehicle.District == EGZCityDistrict::YuexiuOldTown || Vehicle.District == EGZCityDistrict::LiwanXiguan)
	{
		SurfaceSpeedCap = FMath::Min(SurfaceSpeedCap, Config->NarrowLaneSpeedCap);
	}

	// 寻找同方向前车
	float NearestDistance = MAX_FLT;
	const FVector AheadEnd = Vehicle.Location + Vehicle.Forward * Config->SafeDistance;
	for (const FGZTrafficVehicle& Other : Vehicles)
	{
		if (Other.VehicleId == Vehicle.VehicleId || Other.District != Vehicle.District)
		{
			continue;
		}

		const FVector ToOther = Other.Location - Vehicle.Location;
		const float Projection = FVector::DotProduct(ToOther, Vehicle.Forward);
		if (Projection > 0.0f && Projection < Config->SafeDistance)
		{
			const float Lateral = FVector::VectorPlaneProject(ToOther, Vehicle.Forward).Size();
			if (Lateral < 250.0f && Projection < NearestDistance)
			{
				NearestDistance = Projection;
			}
		}
	}

	// 速度决策：前车近则减速，否则趋近目标速度
	float TargetSpeed = SurfaceSpeedCap;
	if (NearestDistance < Config->SafeDistance)
	{
		const float T = FMath::Clamp((NearestDistance - Config->MinFollowingDistance) / (Config->SafeDistance - Config->MinFollowingDistance), 0.0f, 1.0f);
		TargetSpeed = FMath::Lerp(0.0f, SurfaceSpeedCap, T);
	}

	// 鸣笛反应：被鸣笛后短暂加速尝试脱离阻塞
	if (Vehicle.bHornTriggered)
	{
		Vehicle.HornCooldown -= DeltaSeconds;
		if (Vehicle.HornCooldown <= 0.0f)
		{
			Vehicle.bHornTriggered = false;
		}
		else
		{
			TargetSpeed = FMath::Min(TargetSpeed * 1.25f, SurfaceSpeedCap * 1.3f);
		}
	}

	// 简单速度插值
	const float Acceleration = 400.0f;
	Vehicle.Speed = FMath::FInterpConstantTo(Vehicle.Speed, TargetSpeed, DeltaSeconds, Acceleration);

	// 更新位置
	Vehicle.Location += Vehicle.Forward * Vehicle.Speed * DeltaSeconds;

	// 阻塞判定：速度过低且前车很近
	if (Vehicle.Speed < 50.0f && NearestDistance < Config->MinFollowingDistance)
	{
		Vehicle.BlockedTimer += DeltaSeconds;
		if (Vehicle.BlockedTimer >= Config->RerouteBlockedThreshold && !Vehicle.bNeedsReroute)
		{
			Vehicle.bNeedsReroute = true;
			OnVehicleRerouted.Broadcast(Vehicle.District, Vehicle.VehicleId);
		}
	}
	else
	{
		Vehicle.BlockedTimer = 0.0f;
		Vehicle.bNeedsReroute = false;
	}
}

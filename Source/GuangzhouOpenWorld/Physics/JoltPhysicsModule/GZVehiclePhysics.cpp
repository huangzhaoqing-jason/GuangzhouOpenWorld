#include "Physics/JoltPhysicsModule/GZVehiclePhysics.h"
#include "GuangzhouOpenWorld.h"
#include "Math/UnrealMathUtility.h"

FGZVehicleSpec UGZVehiclePhysics::GetDefaultSpec(EGZVehicleType Type)
{
	FGZVehicleSpec S;
	S.Type = Type;

	switch (Type)
	{
	case EGZVehicleType::Sports:
		S.MaxSpeed = 320.0f;
		S.Acceleration = 12.0f;
		S.Braking = 15.0f;
		S.Handling = 0.95f;
		S.Mass = 1400.0f;
		S.Drag = 0.28f;
		S.WheelBase = 2.6f;
		S.TrackWidth = 1.7f;
		S.SuspensionStiffness = 22000.0f;
		S.SuspensionReboundDamping = 850.0f;
		S.SuspensionTravel = 0.08f;
		S.FrontTireGrip = 0.78f;
		S.RearTireGrip = 0.82f;
		S.RainGripMultiplier = 0.45f;
		break;

	case EGZVehicleType::Sedan:
		S.MaxSpeed = 220.0f;
		S.Acceleration = 8.0f;
		S.Braking = 10.0f;
		S.Handling = 0.80f;
		S.Mass = 1600.0f;
		S.Drag = 0.32f;
		S.WheelBase = 2.8f;
		S.TrackWidth = 1.6f;
		S.SuspensionStiffness = 22000.0f;
		S.SuspensionReboundDamping = 850.0f;
		S.SuspensionTravel = 0.12f;
		S.FrontTireGrip = 0.78f;
		S.RearTireGrip = 0.82f;
		S.RainGripMultiplier = 0.45f;
		break;

	case EGZVehicleType::SUV:
		S.MaxSpeed = 200.0f;
		S.Acceleration = 7.0f;
		S.Braking = 9.0f;
		S.Handling = 0.70f;
		S.Mass = 2200.0f;
		S.Drag = 0.38f;
		S.WheelBase = 3.0f;
		S.TrackWidth = 1.75f;
		S.SuspensionStiffness = 22000.0f;
		S.SuspensionReboundDamping = 850.0f;
		S.SuspensionTravel = 0.15f;
		S.FrontTireGrip = 0.78f;
		S.RearTireGrip = 0.82f;
		S.RainGripMultiplier = 0.45f;
		break;

	case EGZVehicleType::Motorcycle:
		S.MaxSpeed = 280.0f;
		S.Acceleration = 14.0f;
		S.Braking = 12.0f;
		S.Handling = 0.98f;
		S.Mass = 250.0f;
		S.Drag = 0.25f;
		S.WheelBase = 1.5f;
		S.TrackWidth = 0.3f;
		S.SuspensionStiffness = 22000.0f;
		S.SuspensionReboundDamping = 850.0f;
		S.SuspensionTravel = 0.10f;
		S.FrontTireGrip = 0.78f;
		S.RearTireGrip = 0.82f;
		S.RainGripMultiplier = 0.45f;
		break;

	case EGZVehicleType::Truck:
		S.MaxSpeed = 140.0f;
		S.Acceleration = 4.0f;
		S.Braking = 7.0f;
		S.Handling = 0.55f;
		S.Mass = 8000.0f;
		S.Drag = 0.55f;
		S.WheelBase = 4.5f;
		S.TrackWidth = 2.0f;
		S.SuspensionStiffness = 22000.0f;
		S.SuspensionReboundDamping = 850.0f;
		S.SuspensionTravel = 0.20f;
		S.FrontTireGrip = 0.78f;
		S.RearTireGrip = 0.82f;
		S.RainGripMultiplier = 0.45f;
		break;
	}

	return S;
}

float UGZVehiclePhysics::GetSurfaceFriction(EGZRoadSurface Surface)
{
	switch (Surface)
	{
	case EGZRoadSurface::Concrete: return 0.85f;
	case EGZRoadSurface::Asphalt:  return 0.79f;
	case EGZRoadSurface::Grass:    return 0.62f;
	case EGZRoadSurface::Water:    return 0.31f;
	default: return 0.85f;
	}
}

UGZVehiclePhysics::UGZVehiclePhysics()
{
	TrafficAIState.CurrentBehavior = EGZTrafficBehavior::Normal;
	TrafficAIState.DecelerationBuffer = 0.4f;
	TrafficAIState.bIsInTunnel = false;
	TrafficAIState.bLightsOn = false;
	TrafficAIState.bWipersOn = false;
	TrafficAIState.CurrentSpeed = 0.0f;
	TrafficAIState.TargetSpeed = 0.0f;
	TrafficAIState.bIsBraking = false;
	TrafficAIState.BrakeTimer = 0.0f;
	TrafficAIState.TurnRadiusMultiplier = 1.0f;
	TrafficAIState.WaterSplashIntensity = 0.0f;

	WaterSplash.bIsActive = false;
	WaterSplash.SplashHeight = 0.0f;
	WaterSplash.SplashWidth = 0.0f;
	WaterSplash.Velocity = FVector::ZeroVector;
	WaterSplash.Lifetime = 0.0f;

	TrafficLight.bIsRed = false;
	TrafficLight.TimeUntilChange = 0.0f;
	TrafficLight.IntersectionLocation = FVector::ZeroVector;
	TrafficLight.bHasTrafficLight = false;

	LightState = EGZVehicleLightState::Auto;
	bIsAIVehicle = true;
	bHasAutoLights = true;
}

void UGZVehiclePhysics::Initialize(EGZVehicleType Type)
{
	Spec = GetDefaultSpec(Type);
	State = FGZVehicleState16DOF();

	float HalfBase = Spec.WheelBase * 0.5f;
	float HalfTrack = Spec.TrackWidth * 0.5f;

	State.WheelFL.Position = FVector(HalfBase, -HalfTrack, 0.0f);
	State.WheelFR.Position = FVector(HalfBase, HalfTrack, 0.0f);
	State.WheelRL.Position = FVector(-HalfBase, -HalfTrack, 0.0f);
	State.WheelRR.Position = FVector(-HalfBase, HalfTrack, 0.0f);

	State.EngineRPM = IdleRPM;
}

void UGZVehiclePhysics::Simulate(float DeltaTime, float ThrottleInput, float BrakeInput, float SteerInput, EGZRoadSurface Surface, bool bIsRaining, bool bIsFoggy, float TimeOfDay)
{
	float SubStep = 1.0f / PhysicsTickRate;
	int32 Steps = FMath::Max(1, FMath::RoundToInt(DeltaTime / SubStep));
	float StepDT = DeltaTime / Steps;

	UpdateTrafficAI(DeltaTime, bIsRaining, bIsFoggy, TimeOfDay);
	UpdateAutoLights(TimeOfDay, bIsRaining, TrafficAIState.bIsInTunnel, bIsFoggy);

	float EffectiveMaxSpeed = Spec.MaxSpeed / 3.6f;
	if (bIsFoggy)
	{
		EffectiveMaxSpeed *= FogSpeedMultiplier;
	}

	for (int32 i = 0; i < Steps; ++i)
	{
		UpdateEngine(StepDT, ThrottleInput);
		UpdateWheels(StepDT, SteerInput, Surface, bIsRaining);
		UpdateSuspension(StepDT);
		UpdateAerodynamics(StepDT);
		UpdateTireWear(StepDT, Surface);
		UpdateDeformationEffects(StepDT);

		float SlopeEffect = CalculateSlopeEffect();
		float SurfaceFriction = GetSurfaceFriction(Surface);
		float EffectiveFriction = SurfaceFriction * State.TireWearOverall * (1.0f - State.Deformation.SuspensionDamage * 0.3f);

		if (bIsRaining)
		{
			EffectiveFriction *= Spec.RainGripMultiplier;
		}

		float EffectiveBraking = Spec.Braking;
		if (bIsRaining)
		{
			EffectiveBraking *= RainBrakingExtension;
		}

		float DriveForce = ThrottleInput * Spec.Acceleration * Spec.Mass * (State.EngineRPM / MaxEngineRPM);
		float BrakeForce = BrakeInput * EffectiveBraking * Spec.Mass;
		float DragForce = State.BodyVelocity * State.BodyVelocity * Spec.Drag * 0.5f * AirDensity * FrontalArea;
		float NetForce = DriveForce - BrakeForce - DragForce;
		NetForce *= (1.0f - SlopeEffect * 0.5f);

		float AccelerationMPS = NetForce / Spec.Mass;
		State.BodyVelocity += AccelerationMPS * StepDT;
		State.BodyVelocity = FMath::Clamp(State.BodyVelocity, 0.0f, EffectiveMaxSpeed);

		FVector Forward = State.Rotation.Vector();
		State.Velocity = Forward * State.BodyVelocity;
		State.Position += State.Velocity * StepDT;

		float EffectiveHandling = Spec.Handling;
		if (bIsRaining)
		{
			EffectiveHandling *= (1.0f / RainTurnRadiusExtension);
		}

		float YawRate = SteerInput * EffectiveHandling * State.BodyVelocity * 0.05f * EffectiveFriction;
		YawRate *= (1.0f - State.Deformation.WheelAlignmentOffset * 0.5f);
		State.Rotation.Yaw += YawRate * StepDT * 180.0f / PI;
	}

	TrafficAIState.CurrentSpeed = State.BodyVelocity;
	UpdateWaterSplash(DeltaTime, State.BodyVelocity);
}

void UGZVehiclePhysics::ApplyDamage(const FVector& ImpactPoint, const FVector& ImpactVelocity, float ImpactMass)
{
	float ImpactSpeed = ImpactVelocity.Size();
	float ImpactSpeedKmh = ImpactSpeed * 3.6f;
	float ImpactEnergy = 0.5f * ImpactMass * ImpactSpeed * ImpactSpeed;
	float DamageFactor = FMath::Clamp(ImpactEnergy / 500000.0f, 0.0f, 1.0f);

	EGZDeformationLevel NewLevel = CalculateDeformationLevel(ImpactSpeedKmh);

	if (NewLevel > State.Deformation.DeformationLevel)
	{
		State.Deformation.DeformationLevel = NewLevel;
	}

	State.Deformation.ChassisIntegrity = FMath::Max(0.0f, State.Deformation.ChassisIntegrity - DamageFactor * 0.3f);
	State.Deformation.WheelAlignmentOffset = FMath::Min(1.0f, State.Deformation.WheelAlignmentOffset + DamageFactor * 0.4f);
	State.Deformation.SuspensionDamage = FMath::Min(1.0f, State.Deformation.SuspensionDamage + DamageFactor * 0.5f);

	UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("Vehicle damage: speed=%.1f km/h, level=%d, chassis=%.2f, alignment=%.2f, suspension=%.2f"),
		ImpactSpeedKmh, (int32)State.Deformation.DeformationLevel, State.Deformation.ChassisIntegrity,
		State.Deformation.WheelAlignmentOffset, State.Deformation.SuspensionDamage);
}

void UGZVehiclePhysics::UpdateEngine(float DeltaTime, float ThrottleInput)
{
	float TargetRPM = IdleRPM + ThrottleInput * (MaxEngineRPM - IdleRPM);
	State.EngineRPM = FMath::FInterpTo(State.EngineRPM, TargetRPM, DeltaTime, 3.0f);
	State.EngineRPM = FMath::Clamp(State.EngineRPM, IdleRPM, MaxEngineRPM);
}

void UGZVehiclePhysics::UpdateWheels(float DeltaTime, float SteerInput, EGZRoadSurface Surface, bool bIsRaining)
{
	float WheelRotationSpeed = State.BodyVelocity / WheelRadius;
	float WheelSlip = WheelRotationSpeed * 0.02f;

	State.WheelFL.Velocity = WheelRotationSpeed;
	State.WheelFR.Velocity = WheelRotationSpeed;
	State.WheelRL.Velocity = WheelRotationSpeed;
	State.WheelRR.Velocity = WheelRotationSpeed;

	State.WheelFL.Position.Y = -Spec.TrackWidth * 0.5f + SteerInput * 0.02f;
	State.WheelFR.Position.Y = Spec.TrackWidth * 0.5f + SteerInput * 0.02f;
}

void UGZVehiclePhysics::UpdateSuspension(float DeltaTime)
{
	float RestLength = Spec.SuspensionTravel;
	float Compression = FMath::Abs(State.BodyVelocity) * 0.001f;

	float SpringForce = Spec.SuspensionStiffness * Compression;
	float DampingForce = Spec.SuspensionReboundDamping * Compression;

	State.WheelFL.SuspensionCompression = FMath::Clamp(Compression * DampingForce / SpringForce, 0.0f, RestLength);
	State.WheelFR.SuspensionCompression = State.WheelFL.SuspensionCompression;
	State.WheelRL.SuspensionCompression = State.WheelFL.SuspensionCompression * 0.8f;
	State.WheelRR.SuspensionCompression = State.WheelFL.SuspensionCompression * 0.8f;
}

void UGZVehiclePhysics::UpdateAerodynamics(float DeltaTime)
{
	float DragForce = State.BodyVelocity * State.BodyVelocity * Spec.Drag * 0.5f * AirDensity * FrontalArea;
	State.BodyVelocity -= (DragForce / Spec.Mass) * DeltaTime;
	State.BodyVelocity = FMath::Max(0.0f, State.BodyVelocity);
}

void UGZVehiclePhysics::UpdateTireWear(float DeltaTime, EGZRoadSurface Surface)
{
	float WearRate = 0.0001f;
	if (Surface == EGZRoadSurface::Grass) WearRate = 0.0003f;
	if (Surface == EGZRoadSurface::Water) WearRate = 0.0005f;

	float SpeedFactor = State.BodyVelocity / (Spec.MaxSpeed / 3.6f);
	WearRate *= (1.0f + SpeedFactor * 2.0f);

	State.WheelFL.TireWear = FMath::Max(0.0f, State.WheelFL.TireWear - WearRate * DeltaTime);
	State.WheelFR.TireWear = FMath::Max(0.0f, State.WheelFR.TireWear - WearRate * DeltaTime);
	State.WheelRL.TireWear = FMath::Max(0.0f, State.WheelRL.TireWear - WearRate * DeltaTime);
	State.WheelRR.TireWear = FMath::Max(0.0f, State.WheelRR.TireWear - WearRate * DeltaTime);

	State.TireWearOverall = (State.WheelFL.TireWear + State.WheelFR.TireWear + State.WheelRL.TireWear + State.WheelRR.TireWear) * 0.25f;
}

void UGZVehiclePhysics::UpdateDeformationEffects(float DeltaTime)
{
	if (State.Deformation.ChassisIntegrity < 0.5f)
	{
		State.BodyVelocity *= (1.0f - 0.1f * DeltaTime);
	}
}

float UGZVehiclePhysics::CalculateSlopeEffect() const
{
	FVector Forward = State.Rotation.Vector();
	float Pitch = State.Rotation.Pitch;
	float SlopeFactor = FMath::Sin(FMath::DegreesToRadians(Pitch));
	return SlopeFactor;
}

EGZDeformationLevel UGZVehiclePhysics::CalculateDeformationLevel(float ImpactSpeedKmh) const
{
	if (ImpactSpeedKmh < 20.0f) return EGZDeformationLevel::None;
	if (ImpactSpeedKmh < 50.0f) return EGZDeformationLevel::Light;
	return EGZDeformationLevel::Heavy;
}

void UGZVehiclePhysics::UpdateTrafficAI(float DeltaTime, bool bIsRaining, bool bIsFoggy, float TimeOfDay)
{
	TrafficAIState.CurrentSpeed = State.BodyVelocity;

	if (bIsFoggy)
	{
		TrafficAIState.TargetSpeed = Spec.MaxSpeed / 3.6f * FogSpeedMultiplier;
		TrafficAIState.CurrentBehavior = EGZTrafficBehavior::NightSparse;
	}
	else
	{
		TrafficAIState.TargetSpeed = Spec.MaxSpeed / 3.6f;
		TrafficAIState.CurrentBehavior = EGZTrafficBehavior::Normal;
	}

	if (bIsRaining)
	{
		TrafficAIState.TurnRadiusMultiplier = RainTurnRadiusExtension;
		TrafficAIState.bWipersOn = true;
	}
	else
	{
		TrafficAIState.TurnRadiusMultiplier = 1.0f;
		TrafficAIState.bWipersOn = false;
	}

	if (bIsFoggy)
	{
		TrafficAIState.TurnRadiusMultiplier = FMath::Max(TrafficAIState.TurnRadiusMultiplier, 1.25f);
	}

	if (TrafficAIState.bIsBraking)
	{
		TrafficAIState.BrakeTimer += DeltaTime;
	}
}

void UGZVehiclePhysics::UpdateAutoLights(float TimeOfDay, bool bIsRaining, bool bIsInTunnel, bool bIsFoggy)
{
	if (!bHasAutoLights)
	{
		return;
	}

	bool bShouldLightsBeOn = false;

	if (TimeOfDay < 6.0f || TimeOfDay > 18.0f)
	{
		bShouldLightsBeOn = true;
	}

	if (bIsRaining)
	{
		bShouldLightsBeOn = true;
	}

	if (bIsInTunnel)
	{
		bShouldLightsBeOn = true;
	}

	if (bIsFoggy)
	{
		bShouldLightsBeOn = true;
	}

	if (bShouldLightsBeOn)
	{
		LightState = EGZVehicleLightState::LowBeam;
		TrafficAIState.bLightsOn = true;
	}
	else
	{
		LightState = EGZVehicleLightState::DaytimeRunning;
		TrafficAIState.bLightsOn = false;
	}
}

void UGZVehiclePhysics::UpdateWaterSplash(float DeltaTime, float VehicleSpeed)
{
	if (VehicleSpeed > WaterSplashThreshold)
	{
		WaterSplash.bIsActive = true;
		float SpeedRatio = (VehicleSpeed - WaterSplashThreshold) / (Spec.MaxSpeed / 3.6f - WaterSplashThreshold);
		SpeedRatio = FMath::Clamp(SpeedRatio, 0.0f, 1.0f);

		WaterSplash.SplashHeight = FMath::Lerp(0.0f, MaxSplashHeight, SpeedRatio);
		WaterSplash.SplashWidth = WaterSplash.SplashHeight * 0.5f;

		FVector Forward = State.Rotation.Vector();
		WaterSplash.Velocity = Forward * VehicleSpeed * 0.3f + FVector(0.0f, 0.0f, VehicleSpeed * 0.5f);

		WaterSplash.Lifetime = FMath::Lerp(1.5f, 0.5f, SpeedRatio);
		TrafficAIState.WaterSplashIntensity = SpeedRatio;
	}
	else
	{
		if (WaterSplash.Lifetime > 0.0f)
		{
			WaterSplash.Lifetime -= DeltaTime;
			if (WaterSplash.Lifetime <= 0.0f)
			{
				WaterSplash.bIsActive = false;
				WaterSplash.SplashHeight = 0.0f;
				WaterSplash.SplashWidth = 0.0f;
				WaterSplash.Velocity = FVector::ZeroVector;
				WaterSplash.Lifetime = 0.0f;
				TrafficAIState.WaterSplashIntensity = 0.0f;
			}
		}
	}
}

void UGZVehiclePhysics::HandleTrafficLight(const FVector& IntersectionLocation, bool bIsRed)
{
	TrafficLight.IntersectionLocation = IntersectionLocation;
	TrafficLight.bIsRed = bIsRed;
	TrafficLight.bHasTrafficLight = true;

	if (bIsRed)
	{
		SmoothBraking(0.0f, 0.016f);
	}
}

void UGZVehiclePhysics::SmoothBraking(float TargetSpeed, float DeltaTime)
{
	TrafficAIState.TargetSpeed = TargetSpeed;
	TrafficAIState.bIsBraking = true;

	if (TrafficAIState.BrakeTimer < TrafficAIState.DecelerationBuffer)
	{
		TrafficAIState.BrakeTimer += DeltaTime;
		return;
	}

	float SpeedDiff = TrafficAIState.CurrentSpeed - TargetSpeed;
	if (SpeedDiff > 0.0f)
	{
		float BrakeStrength = FMath::GetMappedRangeValueClamped(
			FVector2D(0.0f, 1.0f),
			FVector2D(0.1f, 1.0f),
			SpeedDiff / FMath::Max(TrafficAIState.CurrentSpeed, 1.0f)
		);
		State.BodyVelocity = FMath::FInterpTo(State.BodyVelocity, TargetSpeed, DeltaTime, BrakeStrength * 3.0f);
		TrafficAIState.CurrentSpeed = State.BodyVelocity;
	}
	else
	{
		TrafficAIState.bIsBraking = false;
		TrafficAIState.BrakeTimer = 0.0f;
	}
}

void UGZVehiclePhysics::SmoothTurning(float SteerAngle, float DeltaTime)
{
	float SpeedKmh = State.BodyVelocity * 3.6f;
	float SpeedRatio = FMath::Clamp(SpeedKmh / (Spec.MaxSpeed * 0.5f), 0.0f, 1.0f);

	float TurnRadiusFactor = 1.0f + SpeedRatio * 2.0f;
	TurnRadiusFactor *= TrafficAIState.TurnRadiusMultiplier;

	float SmoothedSteer = FMath::FInterpTo(0.0f, SteerAngle, DeltaTime, 2.0f / TurnRadiusFactor);
	SmoothedSteer = FMath::Clamp(SmoothedSteer, -1.0f, 1.0f);

	State.Rotation.Yaw += SmoothedSteer * Spec.Handling * State.BodyVelocity * 0.05f * DeltaTime * 180.0f / PI;
}

UGZVehicleManager::UGZVehicleManager()
{
}

void UGZVehicleManager::Initialize()
{
	ActiveVehicles.Empty();
	NextVehicleID = 0;
	TrafficSpawnTimer = 0.0f;
}

void UGZVehicleManager::Tick(float DeltaTime)
{
	TrafficSpawnTimer += DeltaTime;

	if (TrafficSpawnTimer >= TrafficSpawnInterval && ActiveVehicles.Num() < MaxVehicles)
	{
		TrafficSpawnTimer = 0.0f;

		if (TrafficSpawnPoints.Num() > 0)
		{
			FVector SpawnLoc = TrafficSpawnPoints[FMath::RandRange(0, TrafficSpawnPoints.Num() - 1)];
			FRotator SpawnRot = FRotator(0.0f, FMath::FRandRange(0.0f, 360.0f), 0.0f);
			EGZVehicleType RandType = static_cast<EGZVehicleType>(FMath::RandRange(0, 3));
			SpawnTrafficVehicle(RandType, SpawnLoc, SpawnRot);
		}
	}
}

void UGZVehicleManager::SpawnTrafficVehicle(EGZVehicleType Type, const FVector& Location, const FRotator& Rotation)
{
	if (ActiveVehicles.Num() >= MaxVehicles) return;

	UGZVehiclePhysics* Vehicle = NewObject<UGZVehiclePhysics>(this);
	Vehicle->Initialize(Type);
	FGZVehicleState16DOF State = Vehicle->GetState();
	State.Position = Location;
	State.Rotation = Rotation;

	ActiveVehicles.Add(NextVehicleID, Vehicle);
	NextVehicleID++;

	UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("Spawned traffic vehicle ID=%d, type=%d, total=%d"),
		NextVehicleID - 1, (int32)Type, ActiveVehicles.Num());
}

void UGZVehicleManager::RemoveVehicle(int32 VehicleID)
{
	if (ActiveVehicles.Contains(VehicleID))
	{
		ActiveVehicles.Remove(VehicleID);
	}
}

void UGZVehicleManager::SetMaxVehicles(int32 Max)
{
	MaxVehicles = FMath::Clamp(Max, 0, 200);
}

void UGZVehicleManager::UpdateTrafficLights(float DeltaTime, float TimeOfDay)
{
	for (FGZTrafficLightSystem& Light : TrafficLights)
	{
		Light.TimeUntilChange -= DeltaTime;

		if (Light.TimeUntilChange <= 0.0f)
		{
			Light.bIsRed = !Light.bIsRed;
			Light.TimeUntilChange = Light.bIsRed ? FMath::FRandRange(30.0f, 60.0f) : FMath::FRandRange(20.0f, 45.0f);
		}
	}

	if (TimeOfDay < 6.0f || TimeOfDay > 22.0f)
	{
		for (FGZTrafficLightSystem& Light : TrafficLights)
		{
			Light.bIsRed = false;
			Light.TimeUntilChange = 0.0f;
		}
	}
}

void UGZVehicleManager::SpawnRushHourTraffic(float HourOfDay)
{
	bool bIsRushHour = (HourOfDay >= 7.0f && HourOfDay < 9.0f) || (HourOfDay >= 17.0f && HourOfDay < 19.0f);

	if (bIsRushHour)
	{
		MaxVehicles = FMath::Min(MaxAIUnits, 200);
		TrafficSpawnInterval = 0.5f;
	}
	else
	{
		MaxVehicles = FMath::Min(MaxAIUnits / 2, 200);
		TrafficSpawnInterval = 2.0f;
	}
}

void UGZVehicleManager::SpawnNightSparseTraffic()
{
	int32 NightVehicleCount = FMath::Max(1, FMath::RoundToInt(MaxAIUnits * 0.2f));
	MaxVehicles = FMath::Clamp(NightVehicleCount, 0, 200);
	TrafficSpawnInterval = 5.0f;
}

void UGZVehicleManager::HandleTrafficCongestion()
{
	if (CongestionPoints.Num() == 0)
	{
		return;
	}

	for (const FVector& CongestionPoint : CongestionPoints)
	{
		for (auto& Pair : ActiveVehicles)
		{
			UGZVehiclePhysics* Vehicle = Pair.Value;
			if (!Vehicle) continue;

			const FGZVehicleState16DOF& State = Vehicle->GetState();
			float DistToCongestion = FVector::Dist(State.Position, CongestionPoint);

			if (DistToCongestion < 500.0f)
			{
				Vehicle->SmoothBraking(State.BodyVelocity * 0.3f, 0.016f);
			}
		}
	}
}

void UGZVehicleManager::RerouteTraffic(const FVector& CongestionLocation)
{
	CongestionPoints.AddUnique(CongestionLocation);

	for (auto& Pair : ActiveVehicles)
	{
		UGZVehiclePhysics* Vehicle = Pair.Value;
		if (!Vehicle) continue;

		const FGZVehicleState16DOF& State = Vehicle->GetState();
		float Dist = FVector::Dist(State.Position, CongestionLocation);

		if (Dist < 800.0f)
		{
			Vehicle->TrafficAIState.CurrentBehavior = EGZTrafficBehavior::Rerouting;
			Vehicle->TrafficAIState.TurnRadiusMultiplier = 1.5f;
		}
	}
}
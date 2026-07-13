#include "Physics/GZVehiclePhysics.h"
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
		S.SuspensionStiffness = 35000.0f;
		S.SuspensionDamping = 3500.0f;
		S.SuspensionTravel = 0.08f;
		S.TireFriction = 1.05f;
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
		S.SuspensionStiffness = 28000.0f;
		S.SuspensionDamping = 3000.0f;
		S.SuspensionTravel = 0.12f;
		S.TireFriction = 1.0f;
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
		S.SuspensionStiffness = 25000.0f;
		S.SuspensionDamping = 2800.0f;
		S.SuspensionTravel = 0.15f;
		S.TireFriction = 0.95f;
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
		S.SuspensionStiffness = 15000.0f;
		S.SuspensionDamping = 2000.0f;
		S.SuspensionTravel = 0.10f;
		S.TireFriction = 1.0f;
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
		S.SuspensionStiffness = 50000.0f;
		S.SuspensionDamping = 5000.0f;
		S.SuspensionTravel = 0.20f;
		S.TireFriction = 0.90f;
		break;
	}

	return S;
}

float UGZVehiclePhysics::GetSurfaceFriction(EGZRoadSurface Surface)
{
	switch (Surface)
	{
	case EGZRoadSurface::Concrete: return 1.0f;
	case EGZRoadSurface::Grass:    return 0.6f;
	case EGZRoadSurface::Wet:      return 0.4f;
	default: return 1.0f;
	}
}

UGZVehiclePhysics::UGZVehiclePhysics()
{
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

	State.EngineRPM = 800.0f;
}

void UGZVehiclePhysics::Simulate(float DeltaTime, float ThrottleInput, float BrakeInput, float SteerInput, EGZRoadSurface Surface)
{
	float SubStep = 1.0f / PhysicsTickRate;
	int32 Steps = FMath::Max(1, FMath::RoundToInt(DeltaTime / SubStep));
	float StepDT = DeltaTime / Steps;

	for (int32 i = 0; i < Steps; ++i)
	{
		UpdateEngine(StepDT, ThrottleInput);
		UpdateWheels(StepDT, SteerInput, Surface);
		UpdateSuspension(StepDT);
		UpdateAerodynamics(StepDT);
		UpdateTireWear(StepDT, Surface);
		UpdateDeformationEffects(StepDT);

		float SlopeEffect = CalculateSlopeEffect();
		float SurfaceFriction = GetSurfaceFriction(Surface);
		float EffectiveFriction = SurfaceFriction * State.TireWearOverall * (1.0f - State.Deformation.SuspensionDamage * 0.3f);

		float DriveForce = ThrottleInput * Spec.Acceleration * Spec.Mass * (State.EngineRPM / 7000.0f);
		float BrakeForce = BrakeInput * Spec.Braking * Spec.Mass;
		float DragForce = State.BodyVelocity * State.BodyVelocity * Spec.Drag * 0.5f * 1.225f * 2.2f;
		float NetForce = DriveForce - BrakeForce - DragForce;
		NetForce *= (1.0f - SlopeEffect * 0.5f);

		float AccelerationMPS = NetForce / Spec.Mass;
		State.BodyVelocity += AccelerationMPS * StepDT;
		State.BodyVelocity = FMath::Clamp(State.BodyVelocity, 0.0f, Spec.MaxSpeed / 3.6f);

		FVector Forward = State.Rotation.Vector();
		State.Velocity = Forward * State.BodyVelocity;
		State.Position += State.Velocity * StepDT;

		float YawRate = SteerInput * Spec.Handling * State.BodyVelocity * 0.05f * EffectiveFriction;
		YawRate *= (1.0f - State.Deformation.WheelAlignmentOffset * 0.5f);
		State.Rotation.Yaw += YawRate * StepDT * 180.0f / PI;
	}
}

void UGZVehiclePhysics::ApplyDamage(const FVector& ImpactPoint, const FVector& ImpactVelocity, float ImpactMass)
{
	float ImpactSpeed = ImpactVelocity.Size();
	float ImpactEnergy = 0.5f * ImpactMass * ImpactSpeed * ImpactSpeed;
	float DamageFactor = FMath::Clamp(ImpactEnergy / 500000.0f, 0.0f, 1.0f);

	State.Deformation.ChassisIntegrity = FMath::Max(0.0f, State.Deformation.ChassisIntegrity - DamageFactor * 0.3f);
	State.Deformation.WheelAlignmentOffset = FMath::Min(1.0f, State.Deformation.WheelAlignmentOffset + DamageFactor * 0.4f);
	State.Deformation.SuspensionDamage = FMath::Min(1.0f, State.Deformation.SuspensionDamage + DamageFactor * 0.5f);

	UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("Vehicle damage: chassis=%.2f, alignment=%.2f, suspension=%.2f"),
		State.Deformation.ChassisIntegrity, State.Deformation.WheelAlignmentOffset, State.Deformation.SuspensionDamage);
}

void UGZVehiclePhysics::UpdateEngine(float DeltaTime, float ThrottleInput)
{
	float TargetRPM = 800.0f + ThrottleInput * 6200.0f;
	State.EngineRPM = FMath::FInterpTo(State.EngineRPM, TargetRPM, DeltaTime, 3.0f);
	State.EngineRPM = FMath::Clamp(State.EngineRPM, 800.0f, 7000.0f);
}

void UGZVehiclePhysics::UpdateWheels(float DeltaTime, float SteerInput, EGZRoadSurface Surface)
{
	float SurfaceFric = GetSurfaceFriction(Surface);
	float WheelRotationSpeed = State.BodyVelocity / 0.35f;
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

	State.WheelFL.SuspensionCompression = FMath::Clamp(Compression * Spec.SuspensionDamping / Spec.SuspensionStiffness, 0.0f, RestLength);
	State.WheelFR.SuspensionCompression = State.WheelFL.SuspensionCompression;
	State.WheelRL.SuspensionCompression = State.WheelFL.SuspensionCompression * 0.8f;
	State.WheelRR.SuspensionCompression = State.WheelFL.SuspensionCompression * 0.8f;
}

void UGZVehiclePhysics::UpdateAerodynamics(float DeltaTime)
{
	float DragForce = State.BodyVelocity * State.BodyVelocity * Spec.Drag * 0.5f * 1.225f * 2.2f;
	State.BodyVelocity -= (DragForce / Spec.Mass) * DeltaTime;
	State.BodyVelocity = FMath::Max(0.0f, State.BodyVelocity);
}

void UGZVehiclePhysics::UpdateTireWear(float DeltaTime, EGZRoadSurface Surface)
{
	float WearRate = 0.0001f;
	if (Surface == EGZRoadSurface::Grass) WearRate = 0.0003f;
	if (Surface == EGZRoadSurface::Wet) WearRate = 0.0005f;

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
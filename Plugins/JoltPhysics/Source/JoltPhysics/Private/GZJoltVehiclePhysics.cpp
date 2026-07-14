#include "GZJoltVehiclePhysics.h"

UGZJoltVehiclePhysics::UGZJoltVehiclePhysics()
{
}

void UGZJoltVehiclePhysics::Initialize(const FZJoltVehicleSetup& InSetup)
{
	Setup = InSetup;
	State = FZJoltVehicleState();
}

void UGZJoltVehiclePhysics::ApplyInput(float Throttle, float Brake, float Steering, float DeltaTime)
{
	State.ThrottleInput = FMath::Clamp(Throttle, -1.0f, 1.0f);
	State.BrakeInput = FMath::Clamp(Brake, 0.0f, 1.0f);
	State.SteeringInput = FMath::Clamp(Steering, -1.0f, 1.0f);

	float Traction = GetTractionCoefficient();
	float EngineForce = State.ThrottleInput * 8000.0f * Traction;
	float BrakeDecel = State.BrakeInput * Setup.BrakeForce * Traction / Setup.MassKg;

	State.Velocity += FVector::ForwardVector * EngineForce * DeltaTime / Setup.MassKg;
	State.Velocity -= State.Velocity.GetSafeNormal() * BrakeDecel * DeltaTime * 100.0f;
	State.Velocity *= FMath::Clamp(1.0f - DeltaTime * 0.05f, 0.0f, 1.0f);

	State.SpeedKmh = State.Velocity.Size() * 0.036f;

	if (State.SpeedKmh > 1.0f)
	{
		float SteerFactor = FMath::Clamp(1.0f - State.SpeedKmh / 200.0f, 0.15f, 1.0f);
		float TurnRadius = FMath::Max(5.0f, 12.0f / FMath::Max(SteerFactor, 0.01f));
		float AngularSpeed = State.SpeedKmh / TurnRadius;
		State.BodyRollAngle = State.SteeringInput * FMath::Clamp(State.SpeedKmh / 100.0f, 0.0f, 1.0f) * 5.0f;
	}
	else
	{
		State.BodyRollAngle = 0.0f;
	}

	State.Location += State.Velocity * DeltaTime;
}

void UGZJoltVehiclePhysics::SetRoadCondition(EJoltRoadWetness Wetness, EJoltGroundSurface Surface)
{
	State.RoadWetness = Wetness;
	State.GroundSurface = Surface;
}

float UGZJoltVehiclePhysics::ComputeBrakingDistance() const
{
	float g = 9.81f;
	float Friction = GetSurfaceFriction() * GetWetnessMultiplier();
	if (Friction <= 0.0f) return 999999.0f;
	float v = State.SpeedKmh / 3.6f;
	return (v * v) / (2.0f * g * Friction);
}

float UGZJoltVehiclePhysics::GetTractionCoefficient() const
{
	return GetSurfaceFriction() * GetWetnessMultiplier();
}

float UGZJoltVehiclePhysics::GetSurfaceFriction() const
{
	switch (State.GroundSurface)
	{
	case EJoltGroundSurface::Asphalt: return 0.9f;
	case EJoltGroundSurface::Sidewalk: return 0.75f;
	case EJoltGroundSurface::Greenbelt: return 0.45f;
	default: return 0.8f;
	}
}

float UGZJoltVehiclePhysics::GetWetnessMultiplier() const
{
	switch (State.RoadWetness)
	{
	case EJoltRoadWetness::Dry: return 1.0f;
	case EJoltRoadWetness::Transition: return 0.75f;
	case EJoltRoadWetness::Wet: return 0.5f;
	default: return 1.0f;
	}
}

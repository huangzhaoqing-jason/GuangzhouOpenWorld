#include "Physics/PhysicsWind/GZPhysicsWindComponent.h"
#include "Game/GZGameMode.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"

UGZPhysicsWindComponent::UGZPhysicsWindComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.0f / 60.0f;
	bTickInEditor = false;
}

void UGZPhysicsWindComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	PivotComponent = Owner ? Owner->GetRootComponent() : nullptr;

	LocalVariation = ComputeLocalVariationFromPosition();

	// Random per-instance phase so identical objects never sway in lock-step.
	if (LocalPhaseOffset == 0.0f && Owner)
	{
		const FVector Loc = Owner->GetActorLocation();
		LocalPhaseOffset = FMath::PerlinNoise2D(FVector2D(Loc.X * 0.003f, Loc.Z * 0.003f)) * PI;
	}
}

void UGZPhysicsWindComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

float UGZPhysicsWindComponent::ComputeLocalVariationFromPosition() const
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return 1.0f;
	}

	const FVector Loc = Owner->GetActorLocation();
	const float Hash = FMath::PerlinNoise2D(FVector2D(Loc.X * 0.002f, Loc.Y * 0.002f));
	return 1.0f + Hash * 0.3f;
}

float UGZPhysicsWindComponent::GetMaxSwingAngle() const
{
	AGZGameMode* GZMode = Cast<AGZGameMode>(UGameplayStatics::GetGameMode(this));
	const FPhysicsWindConfig Config = GZMode ? GZMode->GetPhysicsWindConfig() : FPhysicsWindConfig();

	switch (ObjectType)
	{
	case EGZPhysicsWindObjectType::HeavyMetalSign:
		return Config.MaxSignSwingAngle * 0.6f;
	case EGZPhysicsWindObjectType::LightClothBanner:
		return Config.MaxSignSwingAngle * 1.3f;
	case EGZPhysicsWindObjectType::ThickBranch:
		return Config.MaxTreeBranchAngle;
	case EGZPhysicsWindObjectType::ThinBranch:
		return Config.MaxTreeBranchAngle * 1.8f;
	case EGZPhysicsWindObjectType::Leaf:
		return Config.MaxLeafAngle;
	default:
		return Config.MaxTreeBranchAngle;
	}
}

float UGZPhysicsWindComponent::GetNaturalPeriod() const
{
	switch (ObjectType)
	{
	case EGZPhysicsWindObjectType::HeavyMetalSign:
		return 4.5f;
	case EGZPhysicsWindObjectType::LightClothBanner:
		return 2.5f;
	case EGZPhysicsWindObjectType::ThickBranch:
		return 10.0f;
	case EGZPhysicsWindObjectType::ThinBranch:
		return 3.0f;
	case EGZPhysicsWindObjectType::Leaf:
		return 1.2f;
	default:
		return 5.0f;
	}
}

float UGZPhysicsWindComponent::GetMassScale() const
{
	switch (ObjectType)
	{
	case EGZPhysicsWindObjectType::HeavyMetalSign:
		return 4.0f;
	case EGZPhysicsWindObjectType::LightClothBanner:
		return 0.5f;
	case EGZPhysicsWindObjectType::ThickBranch:
		return 3.0f;
	case EGZPhysicsWindObjectType::ThinBranch:
		return 0.8f;
	case EGZPhysicsWindObjectType::Leaf:
		return 0.1f;
	default:
		return 1.0f;
	}
}

float UGZPhysicsWindComponent::GetWindForceMultiplier() const
{
	switch (ObjectType)
	{
	case EGZPhysicsWindObjectType::HeavyMetalSign:
		return 0.3f;
	case EGZPhysicsWindObjectType::LightClothBanner:
		return 2.0f;
	case EGZPhysicsWindObjectType::ThickBranch:
		return 0.2f;
	case EGZPhysicsWindObjectType::ThinBranch:
		return 1.5f;
	case EGZPhysicsWindObjectType::Leaf:
		return 3.0f;
	default:
		return 1.0f;
	}
}

void UGZPhysicsWindComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PivotComponent)
	{
		return;
	}

	AGZGameMode* GZMode = Cast<AGZGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GZMode)
	{
		return;
	}

	const float EffectiveWind = GZMode->GetEffectiveWindStrength();
	const FVegetationWindParams WindParams = GZMode->GetWindParams();
	const FPhysicsWindConfig Config = GZMode->GetPhysicsWindConfig();

	if (!Config.bEnablePhysicsWind)
	{
		return;
	}

	if (EffectiveWind <= 0.001f && FMath::Abs(AngularVelocity) < 0.01f && FMath::Abs(CurrentAngle) < 0.1f)
	{
		CurrentAngle = 0.0f;
		AngularVelocity = 0.0f;
		PivotComponent->SetRelativeRotation(FRotator::ZeroRotator);
		return;
	}

	const float MaxAngle = GetMaxSwingAngle();
	const float Period = GetNaturalPeriod();
	const float Mass = GetMassScale();
	const float ForceMul = GetWindForceMultiplier();

	// Gust impact: rapid wind change adds an angular impulse (subtask 56).
	const float GustTimer = GZMode->GetWindGustTimer();
	const float HighFreqGust = FMath::PerlinNoise1D((GustTimer + LocalPhaseOffset) * 1.5f) * WindParams.WindGustAmplitude;
	const float GustImpulse = HighFreqGust * EffectiveWind * ForceMul * SwingScale * LocalVariation;

	// Base wind torque.
	const float WindTorque = EffectiveWind * ForceMul * SwingScale * LocalVariation * Config.PhysicsForceMultiplier;

	// Restoring force (pendulum gravity / elasticity).
	const float RestoringTorque = -CurrentAngle * SpringStiffness;

	// Damping.
	const float DampingTorque = -AngularVelocity * (1.0f - Config.Damping) * 10.0f;

	// Natural oscillation term (inertia) keeps the motion from looking like a sine wave.
	const float NaturalOscillation = FMath::Sin((GustTimer + LocalPhaseOffset) * 2.0f * PI / Period) * EffectiveWind * ForceMul * SwingScale;

	float AngularAcceleration = (WindTorque + RestoringTorque + DampingTorque + GustImpulse + NaturalOscillation) / Mass;

	// Integrate.
	AngularVelocity += AngularAcceleration * DeltaTime * 60.0f;
	CurrentAngle += AngularVelocity * DeltaTime;

	// Hard clamp to prevent model clipping (subtask 57).
	if (CurrentAngle > MaxAngle)
	{
		CurrentAngle = MaxAngle;
		AngularVelocity = FMath::Min(0.0f, AngularVelocity * -0.3f);
	}
	else if (CurrentAngle < -MaxAngle)
	{
		CurrentAngle = -MaxAngle;
		AngularVelocity = FMath::Max(0.0f, AngularVelocity * -0.3f);
	}

	// Apply rotation around local X axis (swing plane).
	const FRotator NewRotation(CurrentAngle, 0.0f, 0.0f);
	PivotComponent->SetRelativeRotation(NewRotation);
}

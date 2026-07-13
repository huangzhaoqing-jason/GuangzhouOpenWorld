#include "Physics/GZDestructionSystem.h"
#include "GuangzhouOpenWorld.h"
#include "Math/UnrealMathUtility.h"

FGZMaterialDestructionProps UGZDestructionSystem::GetMaterialProps(EGZDestructibleMaterial Material)
{
	FGZMaterialDestructionProps Props;
	Props.Material = Material;

	switch (Material)
	{
	case EGZDestructibleMaterial::Glass:
		Props.FractureToughness = 0.2f;
		Props.FragmentCountMin = 8;
		Props.FragmentCountMax = 32;
		Props.FragmentSizeMin = 0.02f;
		Props.FragmentSizeMax = 0.15f;
		Props.ScatterVelocity = 300.0f;
		break;

	case EGZDestructibleMaterial::Wall:
		Props.FractureToughness = 1.0f;
		Props.FragmentCountMin = 4;
		Props.FragmentCountMax = 20;
		Props.FragmentSizeMin = 0.1f;
		Props.FragmentSizeMax = 0.5f;
		Props.ScatterVelocity = 500.0f;
		break;

	case EGZDestructibleMaterial::Metal:
		Props.FractureToughness = 2.5f;
		Props.FragmentCountMin = 2;
		Props.FragmentCountMax = 10;
		Props.FragmentSizeMin = 0.15f;
		Props.FragmentSizeMax = 0.8f;
		Props.ScatterVelocity = 400.0f;
		break;

	case EGZDestructibleMaterial::Wood:
		Props.FractureToughness = 0.8f;
		Props.FragmentCountMin = 4;
		Props.FragmentCountMax = 16;
		Props.FragmentSizeMin = 0.05f;
		Props.FragmentSizeMax = 0.4f;
		Props.ScatterVelocity = 450.0f;
		break;
	}

	return Props;
}

UGZDestructionSystem::UGZDestructionSystem()
{
}

void UGZDestructionSystem::Initialize(int32 InMaxDebrisLimit)
{
	MaxDebrisLimit = InMaxDebrisLimit;
	ActiveDebris.Empty();
	ActiveDebris.Reserve(MaxDebrisLimit);
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Destruction system initialized: max debris=%d"), MaxDebrisLimit);
}

void UGZDestructionSystem::ApplyDestruction(const FGZImpactData& Impact, EGZDestructibleMaterial Material,
	const FVector& ObjectBoundsMin, const FVector& ObjectBoundsMax)
{
	FGZMaterialDestructionProps Props = GetMaterialProps(Material);

	float ImpactSpeed = Impact.ImpactVelocity.Size();
	float ImpactEnergy = 0.5f * Impact.ImpactMass * ImpactSpeed * ImpactSpeed;
	float RequiredEnergy = Props.FractureToughness * 100000.0f;

	if (ImpactEnergy < RequiredEnergy) return;

	TArray<FGZDebrisFragment> NewFragments;
	GenerateVoronoiFragments(Impact, Props, ObjectBoundsMin, ObjectBoundsMax, NewFragments);

	int32 AvailableSlots = MaxDebrisLimit - ActiveDebris.Num();
	int32 FragmentsToAdd = FMath::Min(NewFragments.Num(), AvailableSlots);

	if (FragmentsToAdd < NewFragments.Num())
	{
		CullExcessDebris();
		AvailableSlots = MaxDebrisLimit - ActiveDebris.Num();
		FragmentsToAdd = FMath::Min(NewFragments.Num(), AvailableSlots);
	}

	for (int32 i = 0; i < FragmentsToAdd; ++i)
	{
		ActiveDebris.Add(NewFragments[i]);
	}
}

void UGZDestructionSystem::Tick(float DeltaTime)
{
	SimulateDebrisPhysics(DeltaTime);
}

void UGZDestructionSystem::SetMaxDebrisLimit(int32 Limit)
{
	MaxDebrisLimit = Limit;
	if (ActiveDebris.Num() > MaxDebrisLimit)
	{
		CullExcessDebris();
	}
}

void UGZDestructionSystem::GenerateVoronoiFragments(const FGZImpactData& Impact, const FGZMaterialDestructionProps& Props,
	const FVector& BoundsMin, const FVector& BoundsMax, TArray<FGZDebrisFragment>& OutFragments)
{
	int32 FragmentCount = FMath::RandRange(Props.FragmentCountMin, Props.FragmentCountMax);
	float ImpactEnergy = 0.5f * Impact.ImpactMass * Impact.ImpactVelocity.SizeSquared();
	float EnergyRatio = FMath::Clamp(ImpactEnergy / (Props.FractureToughness * 100000.0f), 0.0f, 1.0f);
	FragmentCount = FMath::RoundToInt(FragmentCount * (1.0f + EnergyRatio));
	FragmentCount = FMath::Clamp(FragmentCount, Props.FragmentCountMin, Props.FragmentCountMax * 2);

	FVector BoundsSize = BoundsMax - BoundsMin;
	TArray<FVector> SeedPoints;
	SeedPoints.Reserve(FragmentCount);

	for (int32 i = 0; i < FragmentCount; ++i)
	{
		FVector Seed;
		FVector RandomOffset = FVector(
			FMath::FRandRange(-0.3f, 0.3f),
			FMath::FRandRange(-0.3f, 0.3f),
			FMath::FRandRange(-0.3f, 0.3f)
		);
		Seed = Impact.ImpactPoint + RandomOffset * BoundsSize;
		Seed.X = FMath::Clamp(Seed.X, BoundsMin.X, BoundsMax.X);
		Seed.Y = FMath::Clamp(Seed.Y, BoundsMin.Y, BoundsMax.Y);
		Seed.Z = FMath::Clamp(Seed.Z, BoundsMin.Z, BoundsMax.Z);
		SeedPoints.Add(Seed);
	}

	OutFragments.Empty();
	OutFragments.Reserve(FragmentCount);

	for (int32 i = 0; i < FragmentCount; ++i)
	{
		FGZDebrisFragment Fragment;
		Fragment.Position = SeedPoints[i];

		FVector FromImpact = (Fragment.Position - Impact.ImpactPoint).GetSafeNormal();
		if (FromImpact.IsNearlyZero()) FromImpact = Impact.ImpactNormal;

		float ScatterSpeed = Props.ScatterVelocity * EnergyRatio * FMath::FRandRange(0.5f, 1.5f);
		Fragment.Velocity = FromImpact * ScatterSpeed + FVector(
			FMath::FRandRange(-0.3f, 0.3f),
			FMath::FRandRange(-0.3f, 0.3f),
			FMath::FRandRange(0.2f, 0.5f)
		) * ScatterSpeed;

		Fragment.AngularVelocity = FVector(
			FMath::FRandRange(-10.0f, 10.0f),
			FMath::FRandRange(-10.0f, 10.0f),
			FMath::FRandRange(-10.0f, 10.0f)
		);

		float FragSize = FMath::FRandRange(Props.FragmentSizeMin, Props.FragmentSizeMax);
		Fragment.Scale = FVector(FragSize);
		Fragment.Lifetime = 0.0f;
		Fragment.MaxLifetime = FMath::FRandRange(3.0f, 8.0f);
		Fragment.Material = Props.Material;

		OutFragments.Add(Fragment);
	}
}

void UGZDestructionSystem::SimulateDebrisPhysics(float DeltaTime)
{
	for (int32 i = ActiveDebris.Num() - 1; i >= 0; --i)
	{
		FGZDebrisFragment& Debris = ActiveDebris[i];
		Debris.Lifetime += DeltaTime;

		Debris.Velocity.Z -= Gravity * DeltaTime;
		Debris.Velocity *= (1.0f - 0.5f * DeltaTime);
		Debris.AngularVelocity *= (1.0f - 0.3f * DeltaTime);

		Debris.Position += Debris.Velocity * DeltaTime;

		if (Debris.Position.Z < -100.0f || Debris.Lifetime >= Debris.MaxLifetime)
		{
			ActiveDebris.RemoveAtSwap(i);
		}
	}
}

void UGZDestructionSystem::CullExcessDebris()
{
	while (ActiveDebris.Num() > MaxDebrisLimit)
	{
		int32 OldestIdx = 0;
		float OldestLifetime = 0.0f;
		for (int32 i = 0; i < ActiveDebris.Num(); ++i)
		{
			if (ActiveDebris[i].Lifetime > OldestLifetime)
			{
				OldestLifetime = ActiveDebris[i].Lifetime;
				OldestIdx = i;
			}
		}
		ActiveDebris.RemoveAtSwap(OldestIdx);
	}
}
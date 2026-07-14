#include "GZJoltCollisionLayers.h"

void FZJoltCollisionMatrix::InitializeDefault()
{
	for (int32 i = 0; i < 6; ++i)
	{
		bGroundCollides[i] = true;
		bBuildingCollides[i] = true;
		bVehicleCollides[i] = true;
		bNPCCollides[i] = true;
		bPropCollides[i] = true;
		bVegetationCollides[i] = false;
	}

	bVegetationCollides[static_cast<int32>(EJoltCollisionLayer::Ground)] = true;
	bPropCollides[static_cast<int32>(EJoltCollisionLayer::Prop)] = false;
	bVegetationCollides[static_cast<int32>(EJoltCollisionLayer::Vegetation)] = false;
}

bool FZJoltCollisionMatrix::ShouldCollide(EJoltCollisionLayer A, EJoltCollisionLayer B) const
{
	const int32 IndexA = static_cast<int32>(A);
	const int32 IndexB = static_cast<int32>(B);
	if (IndexA < 0 || IndexA >= 6 || IndexB < 0 || IndexB >= 6) return false;

	switch (A)
	{
	case EJoltCollisionLayer::Ground: return bGroundCollides[IndexB];
	case EJoltCollisionLayer::Building: return bBuildingCollides[IndexB];
	case EJoltCollisionLayer::Vehicle: return bVehicleCollides[IndexB];
	case EJoltCollisionLayer::NPC: return bNPCCollides[IndexB];
	case EJoltCollisionLayer::Prop: return bPropCollides[IndexB];
	case EJoltCollisionLayer::Vegetation: return bVegetationCollides[IndexB];
	default: return false;
	}
}

UGZJoltCollisionLayers* UGZJoltCollisionLayers::GetDefaultLayers()
{
	static UGZJoltCollisionLayers* Instance = nullptr;
	if (!Instance)
	{
		Instance = NewObject<UGZJoltCollisionLayers>();
		Instance->Matrix.InitializeDefault();
	}
	return Instance;
}

bool UGZJoltCollisionLayers::ShouldLayersCollide(EJoltCollisionLayer A, EJoltCollisionLayer B) const
{
	return Matrix.ShouldCollide(A, B);
}

bool UGZJoltCollisionLayers::ShouldUseDetailedCollision(EJoltCollisionLayer Layer, float DistanceMeters) const
{
	if (Layer == EJoltCollisionLayer::Prop && DistanceMeters > 800.0f)
	{
		return false;
	}
	if (Layer == EJoltCollisionLayer::Vegetation && DistanceMeters > 400.0f)
	{
		return false;
	}
	return true;
}

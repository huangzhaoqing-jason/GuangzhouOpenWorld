#include "Scene/GSRendering/GZSparkVisibilityManager.h"

UGZSparkVisibilityManager::UGZSparkVisibilityManager()
{
}

void UGZSparkVisibilityManager::Initialize()
{
	bInitialized = true;
	AssetLocations.Empty();
	AssetRadii.Empty();
	AssetVisibilityLevels.Empty();
	HighPriorityAssetIds.Empty();
}

void UGZSparkVisibilityManager::UpdateViewFrustum(const FSparkViewFrustum& Frustum)
{
	CurrentFrustum = Frustum;
	RecomputeVisibility();
}

void UGZSparkVisibilityManager::RegisterAsset(int32 AssetId, const FVector& Location, float Radius)
{
	AssetLocations.Add(AssetId, Location);
	AssetRadii.Add(AssetId, Radius);
	AssetVisibilityLevels.Add(AssetId, 0);
	RecomputeVisibility();
}

void UGZSparkVisibilityManager::UnregisterAsset(int32 AssetId)
{
	AssetLocations.Remove(AssetId);
	AssetRadii.Remove(AssetId);
	AssetVisibilityLevels.Remove(AssetId);
	HighPriorityAssetIds.Remove(AssetId);
}

int32 UGZSparkVisibilityManager::GetVisibilityLevel(int32 AssetId) const
{
	const int32* Level = AssetVisibilityLevels.Find(AssetId);
	return Level ? *Level : 0;
}

void UGZSparkVisibilityManager::RecomputeVisibility()
{
	HighPriorityAssetIds.Empty();

	for (const TPair<int32, FVector>& Pair : AssetLocations)
	{
		const int32 AssetId = Pair.Key;
		const FVector Location = Pair.Value;
		const float Radius = AssetRadii.FindRef(AssetId, 0.0f);

		int32 Level = 0;
		if (IsInsideFrustum(Location, Radius))
		{
			const float Distance = FVector::Dist(CurrentFrustum.Position, Location);
			Level = (Distance <= CurrentFrustum.FarPlane * 0.6f) ? 2 : 1;
			if (Level == 2)
			{
				HighPriorityAssetIds.Add(AssetId);
			}
		}

		AssetVisibilityLevels.Add(AssetId, Level);
	}
}

bool UGZSparkVisibilityManager::IsInsideFrustum(const FVector& Location, float Radius) const
{
	const FVector ToAsset = Location - CurrentFrustum.Position;
	const float Distance = ToAsset.Size();
	if (Distance > CurrentFrustum.FarPlane + Radius)
	{
		return false;
	}

	const FVector Dir = ToAsset.GetSafeNormal();
	const FVector Forward = CurrentFrustum.Direction.GetSafeNormal();
	const float Dot = FVector::DotProduct(Dir, Forward);
	if (Dot < 0.0f)
	{
		return false;
	}

	const float HorizontalAngle = FMath::RadiansToDegrees(FMath::Acos(Dot));
	const float Tolerance = FMath::Max(CurrentFrustum.HorizontalFOV * 0.5f, 5.0f);
	return HorizontalAngle <= Tolerance + FMath::RadiansToDegrees(FMath::Atan2(Radius, Distance));
}

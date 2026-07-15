#include "Scene/GSRendering/GZGSAsset.h"

FBox UGZGSAsset::GetWorldBounds(const FTransform& WorldTransform) const
{
	return LocalBounds.TransformBy(WorldTransform);
}

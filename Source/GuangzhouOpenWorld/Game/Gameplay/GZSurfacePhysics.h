#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZSurfacePhysics.generated.h"

UENUM(BlueprintType)
enum class EGZGuangzhouSurfaceType : uint8
{
	WetCementTile       UMETA(DisplayName = "Wet Cement Tile"),
	OldTownBrick        UMETA(DisplayName = "Old Town Brick"),
	NarrowAlleyAsphalt  UMETA(DisplayName = "Narrow Alley Asphalt"),
	WornConcrete        UMETA(DisplayName = "Worn Concrete"),
	SmoothConcrete      UMETA(DisplayName = "Smooth Concrete"),
	ManholeCover        UMETA(DisplayName = "Manhole Cover"),
	DrainageGrate       UMETA(DisplayName = "Drainage Grate"),
};

USTRUCT()
struct FGZSurfaceProfile
{
	GENERATED_BODY()

	UPROPERTY()
	EGZGuangzhouSurfaceType SurfaceType = EGZGuangzhouSurfaceType::WetCementTile;

	UPROPERTY()
	float BaseFriction = 0.85f;

	UPROPERTY()
	float WetFrictionMultiplier = 0.45f;

	UPROPERTY()
	float PuddleSensitivity = 0.6f;

	UPROPERTY()
	float MaxPuddleDepth = 15.0f;

	UPROPERTY()
	float SpeedPenaltyPerCm = 0.02f;

	UPROPERTY()
	float NPCSpeedMultiplierWet = 0.75f;

	UPROPERTY()
	float BrakingDistanceMultiplierWet = 1.8f;
};

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZSurfacePhysics : public UObject
{
	GENERATED_BODY()

public:
	UGZSurfacePhysics();

	UFUNCTION(BlueprintCallable)
	void Initialize();

	UFUNCTION(BlueprintCallable)
	float GetFriction(EGZGuangzhouSurfaceType Type, float Wetness01, float PuddleDepthCm) const;

	UFUNCTION(BlueprintCallable)
	float GetSpeedPenalty(EGZGuangzhouSurfaceType Type, float PuddleDepthCm) const;

	UFUNCTION(BlueprintCallable)
	float GetNPCSpeedMultiplier(EGZGuangzhouSurfaceType Type, float Wetness01) const;

	UFUNCTION(BlueprintCallable)
	float GetBrakingDistanceMultiplier(EGZGuangzhouSurfaceType Type, float Wetness01) const;

	UFUNCTION(BlueprintCallable)
	float GetMaxPuddleDepth(EGZGuangzhouSurfaceType Type) const;

private:
	void RegisterDefaultProfiles();

	const FGZSurfaceProfile* FindProfile(EGZGuangzhouSurfaceType Type) const;

	UPROPERTY()
	TMap<EGZGuangzhouSurfaceType, FGZSurfaceProfile> Profiles;
};

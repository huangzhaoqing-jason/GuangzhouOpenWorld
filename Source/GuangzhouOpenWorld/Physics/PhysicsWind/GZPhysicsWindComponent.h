#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GZPhysicsWindComponent.generated.h"

UENUM(BlueprintType)
enum class EGZPhysicsWindObjectType : uint8
{
	HeavyMetalSign		UMETA(DisplayName = "Heavy Metal Sign"),
	LightClothBanner	UMETA(DisplayName = "Light Cloth Banner"),
	ThickBranch			UMETA(DisplayName = "Thick Branch"),
	ThinBranch			UMETA(DisplayName = "Thin Branch"),
	Leaf				UMETA(DisplayName = "Leaf"),
};

/**
 * v5.2 Physics-driven wind simulation for signs, banners, trees.
 * Replaces all preset keyframe animations with real-time pendulum physics.
 * Each component self-updates in its own tick, reading global wind parameters
 * from GZGameMode. Max swing angles are enforced to prevent model clipping.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GUANGZHOUOPENWORLD_API UGZPhysicsWindComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGZPhysicsWindComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** What kind of object this component is simulating. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Physics Wind")
	EGZPhysicsWindObjectType ObjectType = EGZPhysicsWindObjectType::ThickBranch;

	/** Per-instance swing intensity multiplier. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Physics Wind")
	float SwingScale = 1.0f;

	/** Per-instance random phase offset so nearby objects do not sway in sync. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Physics Wind")
	float LocalPhaseOffset = 0.0f;

	/** Restitution factor for the simulated spring/pendulum. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Physics Wind")
	float SpringStiffness = 4.0f;

protected:
	/** Current swing angle around the local X axis (degrees). */
	UPROPERTY()
	float CurrentAngle = 0.0f;

	/** Current angular velocity (degrees/second). */
	UPROPERTY()
	float AngularVelocity = 0.0f;

	/** Cached local variation sampled at BeginPlay based on world location. */
	UPROPERTY()
	float LocalVariation = 1.0f;

	/** Cache of the component we are rotating. Falls back to RootComponent. */
	UPROPERTY()
	TObjectPtr<USceneComponent> PivotComponent = nullptr;

	float GetMaxSwingAngle() const;
	float GetNaturalPeriod() const;
	float GetMassScale() const;
	float GetWindForceMultiplier() const;

	/** Compute a deterministic but pseudo-random local variation from world position. */
	float ComputeLocalVariationFromPosition() const;
};

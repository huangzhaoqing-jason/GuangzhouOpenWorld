#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZJoltVehiclePhysics.generated.h"

UENUM(BlueprintType)
enum class EJoltRoadWetness : uint8
{
	Dry         UMETA(DisplayName="Dry"),
	Transition  UMETA(DisplayName="Transition"),
	Wet         UMETA(DisplayName="Wet"),
};

UENUM(BlueprintType)
enum class EJoltGroundSurface : uint8
{
	Asphalt     UMETA(DisplayName="Asphalt"),
	Sidewalk    UMETA(DisplayName="Sidewalk"),
	Greenbelt   UMETA(DisplayName="Greenbelt"),
};

USTRUCT(BlueprintType)
struct FZJoltVehicleSetup
{
	GENERATED_BODY()

	UPROPERTY() float MassKg = 1500.0f;
	UPROPERTY() float SuspensionCompression = 0.3f;
	UPROPERTY() float SuspensionRebound = 0.5f;
	UPROPERTY() float SuspensionDampingDry = 4.5f;
	UPROPERTY() float SuspensionDampingWet = 6.0f;
	UPROPERTY() float WheelRadius = 35.0f;
	UPROPERTY() float MaxSteerAngle = 35.0f;
	UPROPERTY() float BrakeForce = 12000.0f;
};

USTRUCT(BlueprintType)
struct FZJoltVehicleState
{
	GENERATED_BODY()

	UPROPERTY() FVector Location;
	UPROPERTY() FVector Velocity;
	UPROPERTY() float SpeedKmh = 0.0f;
	UPROPERTY() float SteeringInput = 0.0f;
	UPROPERTY() float ThrottleInput = 0.0f;
	UPROPERTY() float BrakeInput = 0.0f;
	UPROPERTY() float BodyRollAngle = 0.0f;
	UPROPERTY() EJoltRoadWetness RoadWetness = EJoltRoadWetness::Dry;
	UPROPERTY() EJoltGroundSurface GroundSurface = EJoltGroundSurface::Asphalt;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class JOLTPHYSICS_API UGZJoltVehiclePhysics : public UObject
{
	GENERATED_BODY()

public:
	UGZJoltVehiclePhysics();

	UFUNCTION(BlueprintCallable, Category="JoltVehicle")
	void Initialize(const FZJoltVehicleSetup& Setup);

	UFUNCTION(BlueprintCallable, Category="JoltVehicle")
	void ApplyInput(float Throttle, float Brake, float Steering, float DeltaTime);

	UFUNCTION(BlueprintCallable, Category="JoltVehicle")
	void SetRoadCondition(EJoltRoadWetness Wetness, EJoltGroundSurface Surface);

	UFUNCTION(BlueprintCallable, Category="JoltVehicle")
	float ComputeBrakingDistance() const;

	UFUNCTION(BlueprintCallable, Category="JoltVehicle")
	float GetTractionCoefficient() const;

	UFUNCTION(BlueprintCallable, Category="JoltVehicle")
	const FZJoltVehicleState& GetState() const { return State; }

private:
	FZJoltVehicleSetup Setup;
	FZJoltVehicleState State;

	float GetSurfaceFriction() const;
	float GetWetnessMultiplier() const;
};

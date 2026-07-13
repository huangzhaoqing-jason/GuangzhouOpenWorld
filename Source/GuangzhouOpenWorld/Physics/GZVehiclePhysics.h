#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZVehiclePhysics.generated.h"

UENUM(BlueprintType)
enum class EGZVehicleType : uint8
{
	Sports		UMETA(DisplayName = "Sports"),
	Sedan		UMETA(DisplayName = "Sedan"),
	SUV			UMETA(DisplayName = "SUV"),
	Motorcycle	UMETA(DisplayName = "Motorcycle"),
	Truck		UMETA(DisplayName = "Truck"),
};

UENUM(BlueprintType)
enum class EGZRoadSurface : uint8
{
	Concrete	UMETA(DisplayName = "Concrete"),
	Grass		UMETA(DisplayName = "Grass"),
	Wet			UMETA(DisplayName = "Wet"),
};

USTRUCT(BlueprintType)
struct FGZVehicleSpec
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZVehicleType Type = EGZVehicleType::Sedan;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Acceleration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Braking = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Handling = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Mass = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Drag = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WheelBase = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TrackWidth = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SuspensionStiffness = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SuspensionDamping = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SuspensionTravel = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TireFriction = 0.0f;
};

USTRUCT(BlueprintType)
struct FGZVehicleDeformation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ChassisIntegrity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WheelAlignmentOffset = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SuspensionDamage = 0.0f;
};

USTRUCT(BlueprintType)
struct FGZWheelState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Velocity = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TireWear = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SuspensionCompression = 0.0f;
};

USTRUCT(BlueprintType)
struct FGZVehicleState16DOF
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector AngularVelocity = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGZWheelState WheelFL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGZWheelState WheelFR;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGZWheelState WheelRL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGZWheelState WheelRR;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BodyVelocity = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EngineRPM = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGZVehicleDeformation Deformation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TireWearOverall = 1.0f;
};

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZVehiclePhysics : public UObject
{
	GENERATED_BODY()

public:
	UGZVehiclePhysics();

	void Initialize(EGZVehicleType Type);
	void Simulate(float DeltaTime, float ThrottleInput, float BrakeInput, float SteerInput, EGZRoadSurface Surface);
	void ApplyDamage(const FVector& ImpactPoint, const FVector& ImpactVelocity, float ImpactMass);

	UFUNCTION(BlueprintPure)
	const FGZVehicleState16DOF& GetState() const { return State; }

	UFUNCTION(BlueprintPure)
	const FGZVehicleSpec& GetSpec() const { return Spec; }

	UFUNCTION(BlueprintPure)
	const FGZVehicleDeformation& GetDeformation() const { return State.Deformation; }

	static FGZVehicleSpec GetDefaultSpec(EGZVehicleType Type);
	static float GetSurfaceFriction(EGZRoadSurface Surface);

private:
	void UpdateEngine(float DeltaTime, float ThrottleInput);
	void UpdateWheels(float DeltaTime, float SteerInput, EGZRoadSurface Surface);
	void UpdateSuspension(float DeltaTime);
	void UpdateAerodynamics(float DeltaTime);
	void UpdateTireWear(float DeltaTime, EGZRoadSurface Surface);
	void UpdateDeformationEffects(float DeltaTime);
	float CalculateSlopeEffect() const;

	UPROPERTY()
	FGZVehicleState16DOF State;

	UPROPERTY()
	FGZVehicleSpec Spec;

	float LastFireTime = 0.0f;
	static constexpr float PhysicsTickRate = 60.0f;
	static constexpr float Gravity = 9.81f;
};

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZVehicleManager : public UObject
{
	GENERATED_BODY()

public:
	UGZVehicleManager();

	void Initialize();
	void Tick(float DeltaTime);
	void SpawnTrafficVehicle(EGZVehicleType Type, const FVector& Location, const FRotator& Rotation);
	void RemoveVehicle(int32 VehicleID);
	void SetMaxVehicles(int32 Max);

	UFUNCTION(BlueprintPure)
	int32 GetActiveVehicleCount() const { return ActiveVehicles.Num(); }

private:
	UPROPERTY()
	TMap<int32, UGZVehiclePhysics*> ActiveVehicles;

	UPROPERTY()
	TArray<FVector> TrafficSpawnPoints;

	int32 MaxVehicles = 200;
	int32 NextVehicleID = 0;
	float TrafficSpawnTimer = 0.0f;
	float TrafficSpawnInterval = 2.0f;
};
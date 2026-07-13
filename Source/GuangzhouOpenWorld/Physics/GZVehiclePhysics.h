// GZVehiclePhysics.h - 16-DOF Vehicle Dynamics for Guangzhou Open World
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GZVehiclePhysics.generated.h"

USTRUCT(BlueprintType)
struct FVehicleConfig
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere) FString Name = TEXT("Sedan");
    UPROPERTY(EditAnywhere) float MaxSpeed = 2000.0f; // cm/s
    UPROPERTY(EditAnywhere) float Acceleration = 500.0f;
    UPROPERTY(EditAnywhere) float Braking = 800.0f;
    UPROPERTY(EditAnywhere) float Handling = 0.035f;
    UPROPERTY(EditAnywhere) float Mass = 1500.0f; // kg
    UPROPERTY(EditAnywhere) float DragCoefficient = 0.3f;
    UPROPERTY(EditAnywhere) float WheelBase = 280.0f; // cm
    UPROPERTY(EditAnywhere) float TrackWidth = 160.0f;
    UPROPERTY(EditAnywhere) float SuspensionStiffness = 30000.0f;
    UPROPERTY(EditAnywhere) float SuspensionDamping = 3000.0f;
    UPROPERTY(EditAnywhere) float SuspensionTravel = 15.0f;
    UPROPERTY(EditAnywhere) float TireFriction = 1.0f;
};

UCLASS()
class GUANGZHOUOPENWORLD_API AGZVehicle : public AActor
{
    GENERATED_BODY()
public:
    AGZVehicle();
    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite) FVehicleConfig VehicleConfig;
    UPROPERTY(BlueprintReadOnly) float CurrentSpeed = 0.0f;
    UPROPERTY(BlueprintReadOnly) float CurrentRPM = 0.0f;
    UPROPERTY(BlueprintReadOnly) float Health = 100.0f;

    UFUNCTION(BlueprintCallable) void Accelerate(float Value);
    UFUNCTION(BlueprintCallable) void Brake(float Value);
    UFUNCTION(BlueprintCallable) void Steer(float Value);
    UFUNCTION(BlueprintCallable) void ApplyDamage(float Damage);

    // 16-DOF suspension state
    UPROPERTY(BlueprintReadOnly) TArray<float> WheelTravel;
    UPROPERTY(BlueprintReadOnly) TArray<float> WheelVelocity;

private:
    // 16 DOF state: x,y,z,roll,pitch,yaw + 4 wheel positions + 4 wheel velocities + body velocity + engine
    FVector BodyVelocity;
    FVector AngularVelocity;
    float EngineRPM = 800.0f;
    float ThrottleInput = 0.0f;
    float BrakeInput = 0.0f;
    float SteerInput = 0.0f;
    void UpdateSuspension(float DeltaTime);
    void UpdateAerodynamics(float DeltaTime);
    void UpdateEngine(float DeltaTime);
};

UCLASS()
class GUANGZHOUOPENWORLD_API AGZVehicleManager : public AActor
{
    GENERATED_BODY()
public:
    AGZVehicleManager();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere) int32 MaxVehicles = 200;
    UPROPERTY(EditAnywhere) TArray<FVehicleConfig> VehicleTypes;

    UFUNCTION(BlueprintCallable) void SpawnRandomVehicle(FVector Location);
    UFUNCTION(BlueprintCallable) void SpawnTrafficVehicles();

private:
    TArray<AGZVehicle*> ActiveVehicles;
    float SpawnTimer = 0.0f;
};
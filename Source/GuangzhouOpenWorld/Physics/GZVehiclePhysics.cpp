#include "GZVehiclePhysics.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AGZVehicle::AGZVehicle()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 1.0f / 60.0f; // 60Hz physics
    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;
    WheelTravel.Init(0.0f, 4);
    WheelVelocity.Init(0.0f, 4);
}

void AGZVehicle::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("Vehicle spawned: %s"), *VehicleConfig.Name);
}

void AGZVehicle::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateEngine(DeltaTime);
    UpdateAerodynamics(DeltaTime);
    UpdateSuspension(DeltaTime);
    // Movement
    float Force = CurrentRPM * VehicleConfig.Acceleration * ThrottleInput - VehicleConfig.Braking * BrakeInput;
    CurrentSpeed += Force / VehicleConfig.Mass * DeltaTime;
    CurrentSpeed = FMath::Clamp(CurrentSpeed, -VehicleConfig.MaxSpeed * 0.3f, VehicleConfig.MaxSpeed);
    if (FMath::Abs(ThrottleInput) < 0.01f && FMath::Abs(BrakeInput) < 0.01f)
        CurrentSpeed *= 0.99f;
    // Steering
    float Turn = SteerInput * VehicleConfig.Handling * (1.0f + FMath::Abs(CurrentSpeed) * 0.0005f);
    FRotator Rot = GetActorRotation();
    Rot.Yaw += Turn * CurrentSpeed * 0.01f * DeltaTime;
    SetActorRotation(Rot);
    // Move
    FVector Forward = GetActorForwardVector();
    AddActorWorldOffset(Forward * CurrentSpeed * DeltaTime);
}

void AGZVehicle::UpdateSuspension(float DeltaTime)
{
    float Stiffness = VehicleConfig.SuspensionStiffness;
    float Damping = VehicleConfig.SuspensionDamping;
    for (int32 i = 0; i < 4; i++)
    {
        float Force = -Stiffness * WheelTravel[i] - Damping * WheelVelocity[i];
        WheelVelocity[i] += Force / (VehicleConfig.Mass * 0.25f) * DeltaTime;
        WheelTravel[i] += WheelVelocity[i] * DeltaTime;
        WheelTravel[i] = FMath::Clamp(WheelTravel[i], -VehicleConfig.SuspensionTravel, VehicleConfig.SuspensionTravel);
    }
}

void AGZVehicle::UpdateAerodynamics(float DeltaTime)
{
    float DragForce = 0.5f * 1.225f * VehicleConfig.DragCoefficient * 2.5f * CurrentSpeed * CurrentSpeed;
    CurrentSpeed -= DragForce / VehicleConfig.Mass * DeltaTime;
}

void AGZVehicle::UpdateEngine(float DeltaTime)
{
    float TargetRPM = 800.0f + FMath::Abs(ThrottleInput) * 7000.0f;
    EngineRPM = FMath::FInterpTo(EngineRPM, TargetRPM, DeltaTime, 3.0f);
    CurrentRPM = EngineRPM + (FMath::Abs(CurrentSpeed) * 3.0f);
}

void AGZVehicle::Accelerate(float Value) { ThrottleInput = FMath::Clamp(Value, 0.0f, 1.0f); BrakeInput = 0.0f; }
void AGZVehicle::Brake(float Value) { BrakeInput = FMath::Clamp(Value, 0.0f, 1.0f); ThrottleInput = 0.0f; }
void AGZVehicle::Steer(float Value) { SteerInput = FMath::Clamp(Value, -1.0f, 1.0f); }
void AGZVehicle::ApplyDamage(float Damage) { Health = FMath::Max(0.0f, Health - Damage); }

AGZVehicleManager::AGZVehicleManager()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 1.0f;
    VehicleTypes = {
        {TEXT("Sports"), 2800, 700, 900, 0.045, 1400, 0.28, 260, 170, 35000, 3500, 12, 1.2},
        {TEXT("Sedan"), 2000, 500, 700, 0.035, 1500, 0.32, 280, 160, 30000, 3000, 15, 1.0},
        {TEXT("SUV"), 1800, 400, 600, 0.025, 2000, 0.38, 290, 170, 35000, 3500, 18, 0.9},
        {TEXT("Motorcycle"), 3200, 900, 800, 0.055, 300, 0.25, 150, 80, 15000, 1500, 8, 1.1},
        {TEXT("Truck"), 1400, 300, 500, 0.018, 5000, 0.55, 350, 200, 50000, 5000, 20, 0.7},
    };
}

void AGZVehicleManager::BeginPlay() { Super::BeginPlay(); SpawnTrafficVehicles(); }
void AGZVehicleManager::Tick(float DeltaTime) { Super::Tick(DeltaTime); SpawnTimer -= DeltaTime; if (SpawnTimer <= 0.0f) { SpawnTimer = 5.0f; SpawnTrafficVehicles(); } }
void AGZVehicleManager::SpawnRandomVehicle(FVector Location)
{
    if (ActiveVehicles.Num() >= MaxVehicles) return;
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    AGZVehicle* V = GetWorld()->SpawnActor<AGZVehicle>(AGZVehicle::StaticClass(), Location, FRotator::ZeroRotator, Params);
    if (V)
    {
        V->VehicleConfig = VehicleTypes[FMath::RandRange(0, VehicleTypes.Num() - 1)];
        ActiveVehicles.Add(V);
    }
}

void AGZVehicleManager::SpawnTrafficVehicles()
{
    for (int32 i = 0; i < 10; i++)
    {
        FVector Loc(FMath::RandRange(-5000, 5000), 0, FMath::RandRange(-5000, 5000));
        SpawnRandomVehicle(Loc);
    }
}
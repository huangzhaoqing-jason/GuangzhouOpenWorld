// GZMassAI.h - Mass AI System for 8000+ NPCs and Vehicles
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GZMassAI.generated.h"

USTRUCT(BlueprintType)
struct FNPCAgent
{
    GENERATED_BODY()
    FVector Position;
    FVector Velocity;
    FVector Destination;
    float Speed = 200.0f;
    float Health = 100.0f;
    int32 Type = 0; // 0=pedestrian, 1=car, 2=police
    float BehaviorTimer = 0.0f;
    bool bActive = true;
};

UCLASS()
class GUANGZHOUOPENWORLD_API AGZMassAIManager : public AActor
{
    GENERATED_BODY()
public:
    AGZMassAIManager();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere) int32 MaxNPCs = 8000;
    UPROPERTY(EditAnywhere) int32 MaxVehicles = 2000;
    UPROPERTY(EditAnywhere) float SimulationRadius = 15000.0f;
    UPROPERTY(EditAnywhere) float CloseUpdateRate = 30.0f; // Hz for nearby agents
    UPROPERTY(EditAnywhere) float FarUpdateRate = 5.0f; // Hz for distant agents

    UFUNCTION(BlueprintCallable) void SpawnNPCCrowd(FVector Center, int32 Count);
    UFUNCTION(BlueprintCallable) void SpawnTrafficFlow(FVector Center, int32 Count);
    UFUNCTION(BlueprintCallable) TArray<FNPCAgent> GetNearbyAgents(FVector Center, float Radius) const;

private:
    TArray<FNPCAgent> Agents;
    float CloseTimer = 0.0f;
    float FarTimer = 0.0f;
    void UpdateAgents(float DeltaTime, bool bClose);
    void UpdateAgentBehavior(FNPCAgent& Agent, float DeltaTime);
    FVector GetRandomDestination(FVector Current) const;
};
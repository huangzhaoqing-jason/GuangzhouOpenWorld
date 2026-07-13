#include "GZMassAI.h"
#include "NavigationSystem.h"

AGZMassAIManager::AGZMassAIManager()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 1.0f / 30.0f;
}

void AGZMassAIManager::BeginPlay()
{
    Super::BeginPlay();
    // Spawn initial crowds
    SpawnNPCCrowd(FVector(0, 0, 0), 3000);
    SpawnTrafficFlow(FVector(0, 0, 0), 1000);
    UE_LOG(LogTemp, Log, TEXT("Mass AI: %d agents initialized"), Agents.Num());
}

void AGZMassAIManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    CloseTimer += DeltaTime;
    FarTimer += DeltaTime;
    if (CloseTimer >= 1.0f / CloseUpdateRate) { CloseTimer = 0.0f; UpdateAgents(DeltaTime, true); }
    if (FarTimer >= 1.0f / FarUpdateRate) { FarTimer = 0.0f; UpdateAgents(DeltaTime, false); }
}

void AGZMassAIManager::UpdateAgents(float DeltaTime, bool bClose)
{
    FVector PlayerPos = FVector::ZeroVector;
    if (GetWorld()->GetFirstPlayerController() && GetWorld()->GetFirstPlayerController()->GetPawn())
        PlayerPos = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
    for (FNPCAgent& Agent : Agents)
    {
        if (!Agent.bActive) continue;
        float Dist = FVector::Dist(Agent.Position, PlayerPos);
        bool bShouldUpdate = bClose ? (Dist < 5000.0f) : (Dist >= 5000.0f && Dist < SimulationRadius);
        if (bShouldUpdate) UpdateAgentBehavior(Agent, DeltaTime);
    }
}

void AGZMassAIManager::UpdateAgentBehavior(FNPCAgent& Agent, float DeltaTime)
{
    Agent.BehaviorTimer -= DeltaTime;
    if (Agent.BehaviorTimer <= 0.0f)
    {
        Agent.Destination = GetRandomDestination(Agent.Position);
        Agent.BehaviorTimer = 3.0f + FMath::FRand() * 5.0f;
    }
    FVector Dir = (Agent.Destination - Agent.Position).GetSafeNormal();
    Agent.Position += Dir * Agent.Speed * DeltaTime;
    if (FVector::Dist(Agent.Position, Agent.Destination) < 100.0f) Agent.BehaviorTimer = 0.0f;
}

FVector AGZMassAIManager::GetRandomDestination(FVector Current) const
{
    return Current + FVector(FMath::RandRange(-2000, 2000), 0, FMath::RandRange(-2000, 2000));
}

void AGZMassAIManager::SpawnNPCCrowd(FVector Center, int32 Count)
{
    for (int32 i = 0; i < Count && Agents.Num() < MaxNPCs; i++)
    {
        FNPCAgent A;
        A.Position = Center + FVector(FMath::RandRange(-5000, 5000), 0, FMath::RandRange(-5000, 5000));
        A.Destination = GetRandomDestination(A.Position);
        A.Speed = 150.0f + FMath::FRand() * 200.0f;
        A.Type = 0;
        Agents.Add(A);
    }
}

void AGZMassAIManager::SpawnTrafficFlow(FVector Center, int32 Count)
{
    for (int32 i = 0; i < Count && Agents.Num() < MaxNPCs; i++)
    {
        FNPCAgent A;
        A.Position = Center + FVector(FMath::RandRange(-5000, 5000), 0, FMath::RandRange(-5000, 5000));
        A.Destination = GetRandomDestination(A.Position);
        A.Speed = 400.0f + FMath::FRand() * 800.0f;
        A.Type = 1;
        Agents.Add(A);
    }
}

TArray<FNPCAgent> AGZMassAIManager::GetNearbyAgents(FVector Center, float Radius) const
{
    TArray<FNPCAgent> Result;
    for (const FNPCAgent& A : Agents)
        if (A.bActive && FVector::Dist(A.Position, Center) < Radius)
            Result.Add(A);
    return Result;
}
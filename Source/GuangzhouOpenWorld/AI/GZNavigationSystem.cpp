#include "GZNavigationSystem.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "NavMesh/RecastNavMesh.h"

UGZNavigationConfig::UGZNavigationConfig()
{
    DefaultAgentName = TEXT("GZAgent");
    NavDataClass = ARecastNavMesh::StaticClass();
}

AGZNavigationManager::AGZNavigationManager()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 1.0f / 30.0f;
}

void AGZNavigationManager::BeginPlay()
{
    Super::BeginPlay();
    NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (NavSys)
    {
        UE_LOG(LogTemp, Log, TEXT("GZ Navigation System initialized. NavMesh: %s"), *NavSys->GetName());
    }
}

void AGZNavigationManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateTimer -= DeltaTime;
    if (UpdateTimer <= 0.0f)
    {
        UpdateTimer = NavUpdateInterval;
        // Periodic nav rebuild for dynamic world
        if (NavSys) NavSys->Build();
    }
}

bool AGZNavigationManager::FindPath(FVector From, FVector To, TArray<FVector>& OutPath)
{
    if (!NavSys) return false;
    UNavigationPath* Path = NavSys->FindPathToLocationSynchronously(GetWorld(), From, To);
    if (Path && Path->IsValid())
    {
        OutPath = Path->PathPoints;
        return true;
    }
    return false;
}

void AGZNavigationManager::RebuildNavigationInRadius(FVector Center, float Radius)
{
    if (NavSys) NavSys->Build();
}
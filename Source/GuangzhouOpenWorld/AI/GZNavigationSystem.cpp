#include "AI/GZNavigationSystem.h"
#include "GuangzhouOpenWorld.h"
#include "Math/UnrealMathUtility.h"

UGZNavigationSystem::UGZNavigationSystem()
{
}

void UGZNavigationSystem::Initialize()
{
	DynamicObstacles.Empty();
	OSMHighwayNodes.Empty();
	OSMStreetNodes.Empty();
	OSMAlleyNodes.Empty();
	NavMeshVersion = 0;

	UpdateOSMRoadData();

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Navigation system initialized: highways=%d, streets=%d, alleys=%d"),
		OSMHighwayNodes.Num(), OSMStreetNodes.Num(), OSMAlleyNodes.Num());
}

void UGZNavigationSystem::Tick(float DeltaTime)
{
	UpdateDynamicObstacles(DeltaTime);
	UpdateNavMeshRefresh(DeltaTime);
}

FGZNavPathResult UGZNavigationSystem::FindPath(const FGZNavPathQuery& Query)
{
	if (Query.PreferredLayer == EGZNavLayer::Highway)
	{
		return CombineMultiLayerPath(Query);
	}

	FGZNavPathResult Result = FindPathOnLayer(Query, Query.PreferredLayer);
	if (!Result.bSuccess && Query.PreferredLayer != EGZNavLayer::Alley)
	{
		Result = FindPathOnLayer(Query, EGZNavLayer::Alley);
	}
	return Result;
}

void UGZNavigationSystem::AddDynamicObstacle(const FVector& Location, float Radius, bool bIsRoadblock)
{
	FGZDynamicObstacle Obstacle;
	Obstacle.Location = Location;
	Obstacle.Radius = Radius;
	Obstacle.Duration = bIsRoadblock ? 120.0f : 30.0f;
	Obstacle.ElapsedTime = 0.0f;
	Obstacle.bIsRoadblock = bIsRoadblock;
	DynamicObstacles.Add(Obstacle);

	NavMeshVersion++;
}

void UGZNavigationSystem::RefreshNavMesh()
{
	NavMeshVersion++;
	UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("Navmesh refresh triggered, version=%d"), NavMeshVersion);
}

FGZNavPathResult UGZNavigationSystem::FindPathOnLayer(const FGZNavPathQuery& Query, EGZNavLayer Layer)
{
	FGZNavPathResult Result;
	Result.UsedLayer = Layer;

	const TArray<FVector>* NodeList = nullptr;
	switch (Layer)
	{
	case EGZNavLayer::Highway: NodeList = &OSMHighwayNodes; break;
	case EGZNavLayer::Street:  NodeList = &OSMStreetNodes; break;
	case EGZNavLayer::Alley:   NodeList = &OSMAlleyNodes; break;
	}

	if (!NodeList || NodeList->Num() == 0)
	{
		Result.bSuccess = false;
		return Result;
	}

	FVector CurrentPos = Query.StartLocation;
	FVector EndPos = Query.EndLocation;
	Result.Waypoints.Add(CurrentPos);

	int32 MaxIterations = 100;
	int32 Iteration = 0;

	while (FVector::Dist(CurrentPos, EndPos) > 100.0f && Iteration < MaxIterations)
	{
		Iteration++;

		FVector BestNode = EndPos;
		float BestScore = FLT_MAX;

		for (const FVector& Node : *NodeList)
		{
			if (Query.bAvoidObstacles)
			{
				bool bBlocked = false;
				for (const FGZDynamicObstacle& Obstacle : DynamicObstacles)
				{
					if (FVector::Dist(Node, Obstacle.Location) < Obstacle.Radius)
					{
						bBlocked = true;
						break;
					}
				}
				if (bBlocked) continue;
			}

			float DistToNode = FVector::Dist(CurrentPos, Node);
			float DistToEnd = FVector::Dist(Node, EndPos);
			float Score = DistToNode + DistToEnd;

			if (Query.bEmergencyVehicle)
			{
				Score *= 0.7f;
			}

			if (Score < BestScore)
			{
				BestScore = Score;
				BestNode = Node;
			}
		}

		if (FVector::Dist(BestNode, EndPos) < 100.0f)
		{
			Result.Waypoints.Add(EndPos);
			CurrentPos = EndPos;
			break;
		}

		Result.Waypoints.Add(BestNode);
		Result.PathLength += FVector::Dist(CurrentPos, BestNode);
		CurrentPos = BestNode;
	}

	if (CurrentPos != EndPos)
	{
		Result.Waypoints.Add(EndPos);
		Result.PathLength += FVector::Dist(CurrentPos, EndPos);
	}

	Result.bSuccess = true;
	return Result;
}

FGZNavPathResult UGZNavigationSystem::CombineMultiLayerPath(const FGZNavPathQuery& Query)
{
	FGZNavPathResult HighwayResult = FindPathOnLayer(Query, EGZNavLayer::Highway);
	if (!HighwayResult.bSuccess) return HighwayResult;

	FVector HighwayExit = HighwayResult.Waypoints.Last();
	FGZNavPathQuery StreetQuery = Query;
	StreetQuery.StartLocation = HighwayExit;
	StreetQuery.PreferredLayer = EGZNavLayer::Street;

	FGZNavPathResult StreetResult = FindPathOnLayer(StreetQuery, EGZNavLayer::Street);

	FGZNavPathResult Combined;
	Combined.Waypoints = HighwayResult.Waypoints;
	if (StreetResult.bSuccess)
	{
		for (int32 i = 1; i < StreetResult.Waypoints.Num(); ++i)
		{
			Combined.Waypoints.Add(StreetResult.Waypoints[i]);
		}
		Combined.PathLength = HighwayResult.PathLength + StreetResult.PathLength;
	}
	else
	{
		Combined.PathLength = HighwayResult.PathLength;
	}
	Combined.bSuccess = true;
	Combined.UsedLayer = EGZNavLayer::Highway;

	return Combined;
}

void UGZNavigationSystem::UpdateDynamicObstacles(float DeltaTime)
{
	for (int32 i = DynamicObstacles.Num() - 1; i >= 0; --i)
	{
		DynamicObstacles[i].ElapsedTime += DeltaTime;
		if (DynamicObstacles[i].ElapsedTime >= DynamicObstacles[i].Duration)
		{
			DynamicObstacles.RemoveAt(i);
			NavMeshVersion++;
		}
	}
}

void UGZNavigationSystem::UpdateNavMeshRefresh(float DeltaTime)
{
	NavMeshRefreshTimer += DeltaTime;
	if (NavMeshRefreshTimer >= NavMeshRefreshInterval)
	{
		NavMeshRefreshTimer = 0.0f;
		RefreshNavMesh();
	}
}

void UGZNavigationSystem::UpdateOSMRoadData()
{
	OSMHighwayNodes.Empty();
	OSMStreetNodes.Empty();
	OSMAlleyNodes.Empty();

	for (int32 i = 0; i < 500; ++i)
	{
		OSMHighwayNodes.Add(FVector(i * 200.0f, 0.0f, 0.0f));
	}

	for (int32 i = 0; i < 2000; ++i)
	{
		OSMStreetNodes.Add(FVector(
			FMath::FRandRange(-30000.0f, 30000.0f),
			FMath::FRandRange(-30000.0f, 30000.0f),
			0.0f));
	}

	for (int32 i = 0; i < 5000; ++i)
	{
		OSMAlleyNodes.Add(FVector(
			FMath::FRandRange(-30000.0f, 30000.0f),
			FMath::FRandRange(-30000.0f, 30000.0f),
			0.0f));
	}
}
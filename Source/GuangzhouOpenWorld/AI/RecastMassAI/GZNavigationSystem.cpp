#include "AI/RecastMassAI/GZNavigationSystem.h"
#include "GuangzhouOpenWorld.h"
#include "Math/UnrealMathUtility.h"
#include "NavigationSystem.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "NavMesh/RecastNavMesh.h"

float UGZNavigationSystem::GetNavLayerCellSize(EGZNavLayer Layer)
{
	switch (Layer)
	{
	case EGZNavLayer::LargeRegion: return LargeCellSize;
	case EGZNavLayer::MediumRegion: return MediumCellSize;
	case EGZNavLayer::FineRegion: return FineCellSize;
	default: return MediumCellSize;
	}
}

float UGZNavigationSystem::GetNavLayerTileSize(EGZNavLayer Layer)
{
	switch (Layer)
	{
	case EGZNavLayer::LargeRegion: return LargeTileSize;
	case EGZNavLayer::MediumRegion: return MediumTileSize;
	case EGZNavLayer::FineRegion: return FineTileSize;
	default: return MediumTileSize;
	}
}

UGZNavigationSystem::UGZNavigationSystem()
{
}

void UGZNavigationSystem::Initialize()
{
	NavTiles.Empty();
	Obstacles.Empty();
	ShelterPoints.Empty();
	NavRefreshTimer = 0.0f;
	ObstacleUpdateTimer = 0.0f;
	SimulationTime = 0.0f;

	InitializeNavLayers();
	FindShelterPoints();

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Navigation: Recast v%.1f, 3 layers: %.2fm/%.2fm/%.2fm, refresh=%.0fHz, obstacle=%.0fms, shelters=%d"),
		RecastVersion, LargeCellSize, MediumCellSize, FineCellSize, NavRefreshRate, ObstacleUpdateInterval * 1000.0f, ShelterPoints.Num());
}

void UGZNavigationSystem::Tick(float DeltaTime)
{
	SimulationTime += DeltaTime;

	NavRefreshTimer += DeltaTime;
	if (NavRefreshTimer >= 1.0f / NavRefreshRate)
	{
		NavRefreshTimer = 0.0f;
		RebuildDirtyTiles();
	}

	ObstacleUpdateTimer += DeltaTime;
	if (ObstacleUpdateTimer >= ObstacleUpdateInterval)
	{
		ObstacleUpdateTimer = 0.0f;
		UpdateObstacles(DeltaTime);
	}
}

void UGZNavigationSystem::RebuildNavMesh()
{
	NavTiles.Empty();
	ShelterPoints.Empty();
	InitializeNavLayers();
	FindShelterPoints();
	RebuildDirtyTiles();
}

void UGZNavigationSystem::AddObstacle(const FGZObstacleData& Obstacle)
{
	FGZObstacleData NewObstacle = Obstacle;
	NewObstacle.UpdateTimestamp = SimulationTime;
	Obstacles.Add(NewObstacle);
	MarkDirtyTiles(Obstacle.Location, Obstacle.Extent);
}

void UGZNavigationSystem::RemoveObstacle(const FVector& Location, const FVector& Extent)
{
	for (int32 i = Obstacles.Num() - 1; i >= 0; --i)
	{
		if (FVector::Dist(Obstacles[i].Location, Location) < Extent.Size())
		{
			MarkDirtyTiles(Obstacles[i].Location, Obstacles[i].Extent);
			Obstacles.RemoveAt(i);
		}
	}
}

bool UGZNavigationSystem::IsPointNavigable(const FVector& Point) const
{
	for (const FGZObstacleData& Obstacle : Obstacles)
	{
		FVector Delta = Point - Obstacle.Location;
		if (FMath::Abs(Delta.X) < Obstacle.Extent.X &&
			FMath::Abs(Delta.Y) < Obstacle.Extent.Y &&
			FMath::Abs(Delta.Z) < Obstacle.Extent.Z)
		{
			return false;
		}
	}
	return true;
}

bool UGZNavigationSystem::FindPath(const FVector& Start, const FVector& End, TArray<FVector>& OutPath, EGZNavLayer Layer) const
{
	OutPath.Empty();

	float CellSize = GetNavLayerCellSize(Layer);
	float Dist = FVector::Dist(Start, End);
	int32 Steps = FMath::Max(2, FMath::RoundToInt(Dist / CellSize));
	FVector Dir = (End - Start).GetSafeNormal();

	for (int32 i = 0; i <= Steps; ++i)
	{
		float T = (float)i / Steps;
		FVector Point = FMath::Lerp(Start, End, T);

		if (!IsPointNavigable(Point))
		{
			FVector Detour = Point + FVector(Dir.Y * CellSize * 2.0f, -Dir.X * CellSize * 2.0f, 0.0f);
			for (int32 attempt = 0; attempt < 8; ++attempt)
			{
				float Angle = (attempt * 45.0f) * PI / 180.0f;
				FVector DetourOffset = FVector(
					FMath::Cos(Angle) * CellSize * 3.0f,
					FMath::Sin(Angle) * CellSize * 3.0f,
					0.0f
				);
				FVector DetourPoint = Point + DetourOffset;
				if (IsPointNavigable(DetourPoint))
				{
					OutPath.Add(DetourPoint);
					break;
				}
			}
		}
		else
		{
			OutPath.Add(Point);
		}
	}

	return OutPath.Num() > 0;
}

void UGZNavigationSystem::FindShelterPoints()
{
	ShelterPoints.Empty();

	float MapHalf = 10000.0f;
	float GridStep = 2000.0f;

	for (float X = -MapHalf; X < MapHalf; X += GridStep)
	{
		for (float Y = -MapHalf; Y < MapHalf; Y += GridStep)
		{
			FVector BasePoint(X, Y, 0.0f);

			if (IsPointNavigable(BasePoint))
			{
				ShelterPoints.Add(BasePoint);

				FVector Entrance1(X + 500.0f, Y, 0.0f);
				if (IsPointNavigable(Entrance1))
				{
					ShelterPoints.Add(Entrance1);
				}

				FVector Entrance2(X, Y + 500.0f, 0.0f);
				if (IsPointNavigable(Entrance2))
				{
					ShelterPoints.Add(Entrance2);
				}

				FVector Entrance3(X - 500.0f, Y, 0.0f);
				if (IsPointNavigable(Entrance3))
				{
					ShelterPoints.Add(Entrance3);
				}

				FVector Entrance4(X, Y - 500.0f, 0.0f);
				if (IsPointNavigable(Entrance4))
				{
					ShelterPoints.Add(Entrance4);
				}
			}
		}
	}

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Shelter points: found %d potential shelter locations (building entrances, covered areas)"),
		ShelterPoints.Num());
}

FVector UGZNavigationSystem::FindNearestShelter(const FVector& Location) const
{
	FVector Nearest = Location;
	float NearestDist = ShelterSearchRadius;

	for (const FVector& Shelter : ShelterPoints)
	{
		float Dist = FVector::Dist(Location, Shelter);
		if (Dist < NearestDist)
		{
			NearestDist = Dist;
			Nearest = Shelter;
		}
	}

	return Nearest;
}

void UGZNavigationSystem::UpdateObstacles(float DeltaTime)
{
	for (int32 i = Obstacles.Num() - 1; i >= 0; --i)
	{
		if (Obstacles[i].bIsDynamic)
		{
			float Age = SimulationTime - Obstacles[i].UpdateTimestamp;
			if (Age > 10.0f)
			{
				MarkDirtyTiles(Obstacles[i].Location, Obstacles[i].Extent);
				Obstacles.RemoveAt(i);
			}
		}
	}
}

void UGZNavigationSystem::MarkDirtyTiles(const FVector& Location, const FVector& Extent)
{
	for (FGZNavMeshTile& Tile : NavTiles)
	{
		FVector2D Loc2D(Location.X, Location.Y);
		if (Loc2D.X >= Tile.TileMin.X - Extent.X && Loc2D.X <= Tile.TileMax.X + Extent.X &&
			Loc2D.Y >= Tile.TileMin.Y - Extent.Y && Loc2D.Y <= Tile.TileMax.Y + Extent.Y)
		{
			Tile.bIsDirty = true;
		}
	}
}

void UGZNavigationSystem::RebuildDirtyTiles()
{
	int32 RebuiltCount = 0;
	for (FGZNavMeshTile& Tile : NavTiles)
	{
		if (Tile.bIsDirty)
		{
			Tile.bIsDirty = false;
			Tile.LastUpdateTime = SimulationTime;
			RebuiltCount++;
		}
	}

	if (RebuiltCount > 0)
	{
		UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("Nav: rebuilt %d tiles"), RebuiltCount);
	}
}

void UGZNavigationSystem::InitializeNavLayers()
{
	float MapSize = 20000.0f;
	float HalfMap = MapSize * 0.5f;

	for (float X = -HalfMap; X < HalfMap; X += LargeTileSize)
	{
		for (float Y = -HalfMap; Y < HalfMap; Y += LargeTileSize)
		{
			FGZNavMeshTile Tile;
			Tile.TileMin = FVector2D(X, Y);
			Tile.TileMax = FVector2D(X + LargeTileSize, Y + LargeTileSize);
			Tile.Layer = EGZNavLayer::LargeRegion;
			Tile.bIsDirty = true;
			NavTiles.Add(Tile);
		}
	}

	for (float X = -HalfMap; X < HalfMap; X += MediumTileSize)
	{
		for (float Y = -HalfMap; Y < HalfMap; Y += MediumTileSize)
		{
			FGZNavMeshTile Tile;
			Tile.TileMin = FVector2D(X, Y);
			Tile.TileMax = FVector2D(X + MediumTileSize, Y + MediumTileSize);
			Tile.Layer = EGZNavLayer::MediumRegion;
			Tile.bIsDirty = true;
			NavTiles.Add(Tile);
		}
	}

	for (float X = -HalfMap; X < HalfMap; X += FineTileSize)
	{
		for (float Y = -HalfMap; Y < HalfMap; Y += FineTileSize)
		{
			FGZNavMeshTile Tile;
			Tile.TileMin = FVector2D(X, Y);
			Tile.TileMax = FVector2D(X + FineTileSize, Y + FineTileSize);
			Tile.Layer = EGZNavLayer::FineRegion;
			Tile.bIsDirty = true;
			NavTiles.Add(Tile);
		}
	}
}

void UGZNavigationSystem::RegisterOSMData()
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Registering 1:1 OSM Guangzhou data, no simplification"));
}
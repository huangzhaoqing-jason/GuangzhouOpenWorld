#include "Scene/GZWorldPartition.h"
#include "GuangzhouOpenWorld.h"
#include "Math/UnrealMathUtility.h"

UGZWorldPartition::UGZWorldPartition()
{
}

void UGZWorldPartition::Initialize(float InCellSize)
{
	CellSize = InCellSize;
	Cells.Empty();
	CurrentOriginX = 0.0f;
	CurrentOriginY = 0.0f;
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("World Partition initialized: cellSize=%.0fcm, rebaseDistance=%.0fcm"),
		CellSize, OriginRebaseDistance);
}

void UGZWorldPartition::Tick(float DeltaTime, const FVector& PlayerPosition)
{
	CheckOriginRebasing(PlayerPosition);
	UpdateCellLODs(PlayerPosition);
	PreloadNearbyCells(PlayerPosition);
	UnloadDistantCells(PlayerPosition);

	MemoryCheckTimer += DeltaTime;
	if (MemoryCheckTimer >= 5.0f)
	{
		MemoryCheckTimer = 0.0f;
		UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("World Partition: %d cells loaded"), GetLoadedCellCount());
	}
}

void UGZWorldPartition::SetOriginRebaseDistance(float Distance)
{
	OriginRebaseDistance = FMath::Max(Distance, CellSize * 10.0f);
}

void UGZWorldPartition::ForceLoadCell(const FIntVector2& GridCoord)
{
	if (!Cells.Contains(GridCoord))
	{
		FGZWorldCell Cell;
		Cell.GridCoord = GridCoord;
		Cell.WorldCenter = GridToWorld(GridCoord);
		Cells.Add(GridCoord, Cell);
	}

	FGZWorldCell& Cell = Cells[GridCoord];
	Cell.bIsLoaded = true;
	Cell.bRenderDataLoaded = true;
	Cell.bPhysicsDataLoaded = true;
	Cell.bNavMeshLoaded = true;
	Cell.LastAccessTime = 0.0f;
}

void UGZWorldPartition::UnloadCell(const FIntVector2& GridCoord)
{
	if (Cells.Contains(GridCoord))
	{
		FGZWorldCell& Cell = Cells[GridCoord];
		Cell.bRenderDataLoaded = false;
		Cell.bPhysicsDataLoaded = false;
		Cell.bIsLoaded = false;
	}
}

int32 UGZWorldPartition::GetLoadedCellCount() const
{
	int32 Count = 0;
	for (const auto& Pair : Cells)
	{
		if (Pair.Value.bIsLoaded) Count++;
	}
	return Count;
}

void UGZWorldPartition::UpdateCellLODs(const FVector& PlayerPosition)
{
	for (auto& Pair : Cells)
	{
		FGZWorldCell& Cell = Pair.Value;
		Cell.DistanceFromPlayer = FVector::Dist2D(PlayerPosition, Cell.WorldCenter);

		if (Cell.DistanceFromPlayer < CellSize)
		{
			Cell.LODLevel = 0;
		}
		else if (Cell.DistanceFromPlayer < CellSize * 3)
		{
			Cell.LODLevel = 1;
		}
		else if (Cell.DistanceFromPlayer < CellSize * 6)
		{
			Cell.LODLevel = 2;
		}
		else
		{
			Cell.LODLevel = 3;
		}
	}
}

void UGZWorldPartition::PreloadNearbyCells(const FVector& PlayerPosition)
{
	FIntVector2 PlayerCell = WorldToGrid(PlayerPosition);

	for (int32 Y = -PreloadRadius; Y <= PreloadRadius; ++Y)
	{
		for (int32 X = -PreloadRadius; X <= PreloadRadius; ++X)
		{
			FIntVector2 Coord(PlayerCell.X + X, PlayerCell.Y + Y);

			if (!Cells.Contains(Coord))
			{
				FGZWorldCell Cell;
				Cell.GridCoord = Coord;
				Cell.WorldCenter = GridToWorld(Coord);
				Cell.LODLevel = FMath::Max(FMath::Abs(X), FMath::Abs(Y));
				Cells.Add(Coord, Cell);
			}

			FGZWorldCell& Cell = Cells[Coord];
			if (!Cell.bIsLoaded)
			{
				Cell.bIsLoaded = true;
				Cell.bRenderDataLoaded = (Cell.LODLevel <= 1);
				Cell.bPhysicsDataLoaded = (Cell.LODLevel <= 1);
				Cell.bNavMeshLoaded = true;
			}
		}
	}
}

void UGZWorldPartition::UnloadDistantCells(const FVector& PlayerPosition)
{
	float UnloadDistance = CellSize * (PreloadRadius + 2);

	for (auto& Pair : Cells)
	{
		FGZWorldCell& Cell = Pair.Value;
		float Dist = FVector::Dist2D(PlayerPosition, Cell.WorldCenter);

		if (Dist > UnloadDistance && Cell.bIsLoaded)
		{
			Cell.bRenderDataLoaded = false;
			Cell.bPhysicsDataLoaded = false;
			Cell.bIsLoaded = false;
		}
	}
}

void UGZWorldPartition::CheckOriginRebasing(const FVector& PlayerPosition)
{
	float DistFromOrigin = FVector::Dist2D(PlayerPosition, FVector(CurrentOriginX, CurrentOriginY, 0.0f));

	if (DistFromOrigin > OriginRebaseDistance)
	{
		CurrentOriginX = PlayerPosition.X;
		CurrentOriginY = PlayerPosition.Y;

		for (auto& Pair : Cells)
		{
			FGZWorldCell& Cell = Pair.Value;
			Cell.WorldCenter = GridToWorld(Cell.GridCoord);
		}

		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Origin rebased to (%.0f, %.0f)"), CurrentOriginX, CurrentOriginY);
	}
}

FIntVector2 UGZWorldPartition::WorldToGrid(const FVector& WorldPos) const
{
	float LocalX = WorldPos.X - CurrentOriginX;
	float LocalY = WorldPos.Y - CurrentOriginY;
	return FIntVector2(
		FMath::FloorToInt(LocalX / CellSize),
		FMath::FloorToInt(LocalY / CellSize));
}

FVector UGZWorldPartition::GridToWorld(const FIntVector2& GridCoord) const
{
	return FVector(
		(GridCoord.X + 0.5f) * CellSize + CurrentOriginX,
		(GridCoord.Y + 0.5f) * CellSize + CurrentOriginY,
		0.0f);
}
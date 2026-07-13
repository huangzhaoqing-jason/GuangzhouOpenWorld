#include "Scene/RenderingSystem/GZWorldPartition.h"
#include "GuangzhouOpenWorld.h"
#include "Math/UnrealMathUtility.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "HAL/ThreadManager.h"

UGZWorldPartition::UGZWorldPartition()
{
}

void UGZWorldPartition::Initialize()
{
	Cells.Empty();
	LoadQueue.Empty();
	LoadQueue.Reserve(MaxLoadQueueSize);

	int32 HalfX = GridSizeX / 2;
	int32 HalfY = GridSizeY / 2;

	for (int32 X = -HalfX; X < HalfX; ++X)
	{
		for (int32 Y = -HalfY; Y < HalfY; ++Y)
		{
			FGZWorldCell Cell;
			Cell.GridCoord = FIntVector2(X, Y);
			Cell.CenterPosition = FVector(X * CellSize, Y * CellSize, 0.0f);
			Cell.bIsLoaded = false;
			Cells.Add(Cell);
		}
	}

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("World Partition: %dx%d grid, %.0fm cells, %d preload, origin shift=%.0f, %d threads on cores %d/%d/%d"),
		GridSizeX, GridSizeY, CellSize / 100.0f, PreloadCells, WorldOriginShiftThreshold, ThreadCount, PerfCore0, PerfCore1, PerfCore2);
}

void UGZWorldPartition::Tick(float DeltaTime, const FVector& PlayerPosition)
{
	LastPlayerPosition = PlayerPosition;
	PlayerCellCoord = GetCellAtPosition(PlayerPosition);

	UpdateWorldOrigin(PlayerPosition);
	UpdateCellLoading(PlayerPosition);

	LoadQueueTimer += DeltaTime;
	if (LoadQueueTimer >= LoadQueueInterval)
	{
		LoadQueueTimer = 0.0f;
		ProcessCellQueue();
	}

	CellReleaseTimer += DeltaTime;
	if (CellReleaseTimer >= CellReleaseInterval)
	{
		CellReleaseTimer = 0.0f;
		ReleaseDistantCells(PlayerPosition);
	}
}

void UGZWorldPartition::SetCellSize(float InCellSize)
{
	CellSize = InCellSize;
}

void UGZWorldPartition::SetPreloadRadius(int32 InPreloadCells)
{
	PreloadCells = FMath::Clamp(InPreloadCells, 1, 8);
}

void UGZWorldPartition::ForceLoadCell(const FIntVector2& GridCoord)
{
	LoadCell(GridCoord);
}

void UGZWorldPartition::ForceUnloadCell(const FIntVector2& GridCoord)
{
	UnloadCell(GridCoord);
}

void UGZWorldPartition::SetLightingForCell(const FIntVector2& GridCoord, float Intensity, const FLinearColor& Color)
{
	for (FGZWorldCell& Cell : Cells)
	{
		if (Cell.GridCoord == GridCoord)
		{
			Cell.LightingIntensity = Intensity;
			Cell.LightingColor = Color;
			Cell.bLightingReady = true;
			return;
		}
	}
}

int32 UGZWorldPartition::GetLoadedCellCount() const
{
	int32 Count = 0;
	for (const FGZWorldCell& Cell : Cells)
	{
		if (Cell.bIsLoaded) Count++;
	}
	return Count;
}

FIntVector2 UGZWorldPartition::GetCellAtPosition(const FVector& WorldPosition) const
{
	return FIntVector2(
		FMath::RoundToInt(WorldPosition.X / CellSize),
		FMath::RoundToInt(WorldPosition.Y / CellSize)
	);
}

void UGZWorldPartition::UpdateCellLoading(const FVector& PlayerPosition)
{
	for (int32 X = -PreloadCells; X <= PreloadCells; ++X)
	{
		for (int32 Y = -PreloadCells; Y <= PreloadCells; ++Y)
		{
			FIntVector2 Coord = FIntVector2(PlayerCellCoord.X + X, PlayerCellCoord.Y + Y);
			bool bCellExists = false;

			for (FGZWorldCell& Cell : Cells)
			{
				if (Cell.GridCoord == Coord)
				{
					bCellExists = true;
					if (!Cell.bIsLoaded && LoadQueue.Num() < MaxLoadQueueSize)
					{
						LoadQueue.AddUnique(Coord);
					}
					break;
				}
			}
		}
	}
}

void UGZWorldPartition::LoadCell(const FIntVector2& GridCoord)
{
	for (FGZWorldCell& Cell : Cells)
	{
		if (Cell.GridCoord == GridCoord && !Cell.bIsLoaded)
		{
			Cell.bIsLoaded = true;
			Cell.LastAccessTime = 0.0f;
			AssignLoadPriority(Cell);
			RunLoadThread(Cell);
			return;
		}
	}
}

void UGZWorldPartition::UnloadCell(const FIntVector2& GridCoord)
{
	for (FGZWorldCell& Cell : Cells)
	{
		if (Cell.GridCoord == GridCoord && Cell.bIsLoaded)
		{
			Cell.bNaniteReady = false;
			Cell.bLumenReady = false;
			Cell.bNavmeshReady = false;
			Cell.bDetailsReady = false;
			Cell.bIsLoaded = false;
			return;
		}
	}
}

void UGZWorldPartition::ProcessCellQueue()
{
	while (LoadQueue.Num() > 0)
	{
		FIntVector2 Coord = LoadQueue[0];
		LoadQueue.RemoveAt(0);
		LoadCell(Coord);
	}

	for (FGZWorldCell& Cell : Cells)
	{
		if (Cell.bIsLoaded)
		{
			Cell.LastAccessTime += LoadQueueInterval;
		}
	}
}

void UGZWorldPartition::UpdateWorldOrigin(const FVector& PlayerPosition)
{
	WorldOriginCheckTimer += LoadQueueInterval;
	if (WorldOriginCheckTimer < WorldOriginCheckInterval) return;
	WorldOriginCheckTimer = 0.0f;

	FVector Delta = PlayerPosition - WorldOrigin;
	if (Delta.Size() > WorldOriginShiftThreshold)
	{
		WorldOrigin = PlayerPosition;
		UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("World origin shifted to: (%.0f, %.0f, %.0f)"),
			WorldOrigin.X, WorldOrigin.Y, WorldOrigin.Z);
	}
}

void UGZWorldPartition::ReleaseDistantCells(const FVector& PlayerPosition)
{
	for (FGZWorldCell& Cell : Cells)
	{
		if (!Cell.bIsLoaded) continue;

		FIntVector2 Delta = FIntVector2(Cell.GridCoord.X - PlayerCellCoord.X, Cell.GridCoord.Y - PlayerCellCoord.Y);
		int32 Dist = FMath::Max(FMath::Abs(Delta.X), FMath::Abs(Delta.Y));

		if (Dist > ReleaseDistance)
		{
			Cell.bNaniteReady = false;
			Cell.bLumenReady = false;
			Cell.bDetailsReady = false;
		}
	}
}

void UGZWorldPartition::AssignLoadPriority(FGZWorldCell& Cell)
{
	if (!Cell.bNaniteReady)
	{
		Cell.CurrentLoadPriority = EGZCellPriority::Nanite;
	}
	else if (!Cell.bLumenReady)
	{
		Cell.CurrentLoadPriority = EGZCellPriority::Lumen;
	}
	else if (!Cell.bNavmeshReady)
	{
		Cell.CurrentLoadPriority = EGZCellPriority::Navmesh;
	}
	else if (!Cell.bDetailsReady)
	{
		Cell.CurrentLoadPriority = EGZCellPriority::Details;
	}
}

void UGZWorldPartition::RunLoadThread(FGZWorldCell& Cell)
{
	int32 CellIndex = &Cell - &Cells[0];
	ThreadTask_Nanite(CellIndex);
	ThreadTask_Lumen(CellIndex);
	ThreadTask_Navmesh(CellIndex);
}

void UGZWorldPartition::ThreadTask_Nanite(int32 CellIndex)
{
	if (CellIndex >= 0 && CellIndex < Cells.Num())
	{
		Cells[CellIndex].bNaniteReady = true;
	}
}

void UGZWorldPartition::ThreadTask_Lumen(int32 CellIndex)
{
	if (CellIndex >= 0 && CellIndex < Cells.Num())
	{
		Cells[CellIndex].bLumenReady = true;
	}
}

void UGZWorldPartition::ThreadTask_Navmesh(int32 CellIndex)
{
	if (CellIndex >= 0 && CellIndex < Cells.Num())
	{
		Cells[CellIndex].bNavmeshReady = true;
		Cells[CellIndex].bDetailsReady = true;
	}
}
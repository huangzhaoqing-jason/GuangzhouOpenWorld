#include "GZWorldPartition.h"
#include "Engine/World.h"
#include "WorldPartition/WorldPartition.h"

AGZWorldPartitionManager::AGZWorldPartitionManager()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 0.5f;
}

void AGZWorldPartitionManager::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("World Partition Manager: Grid=%.0fm, Preload=%d cells"), GridSize / 100.0f, PreloadCells);
    if (bEnableOriginRebasing)
    {
        UE_LOG(LogTemp, Log, TEXT("Origin Rebasing: Enabled (threshold=%.0fm)"), OriginRebaseThreshold / 100.0f);
    }
}

void AGZWorldPartitionManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateStreaming();
}

void AGZWorldPartitionManager::UpdateStreaming()
{
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        if (APawn* Pawn = PC->GetPawn())
        {
            FVector Loc = Pawn->GetActorLocation();
            FIntPoint NewCell(FMath::FloorToInt(Loc.X / GridSize), FMath::FloorToInt(Loc.Z / GridSize));
            if (NewCell != CurrentCell)
            {
                CurrentCell = NewCell;
                // Load cells in range
                for (int32 x = -PreloadCells; x <= PreloadCells; x++)
                {
                    for (int32 y = -PreloadCells; y <= PreloadCells; y++)
                    {
                        FIntPoint Cell(CurrentCell.X + x, CurrentCell.Y + y);
                        if (!LoadedCells.Contains(Cell)) LoadCell(Cell);
                    }
                }
                // Unload distant cells
                for (int32 i = LoadedCells.Num() - 1; i >= 0; i--)
                {
                    if (FMath::Abs(LoadedCells[i].X - CurrentCell.X) > PreloadCells + 2 ||
                        FMath::Abs(LoadedCells[i].Y - CurrentCell.Y) > PreloadCells + 2)
                        UnloadCell(LoadedCells[i]);
                }
                // Origin rebasing
                if (bEnableOriginRebasing && Loc.Size() > OriginRebaseThreshold)
                {
                    FVector Offset = FVector(FMath::RoundToFloat(Loc.X / GridSize) * GridSize, 0,
                        FMath::RoundToFloat(Loc.Z / GridSize) * GridSize);
                    UE_LOG(LogTemp, Log, TEXT("Origin rebase to: %.0f, %.0f"), Offset.X, Offset.Z);
                }
            }
        }
    }
}

void AGZWorldPartitionManager::LoadCell(FIntPoint CellCoord)
{
    LoadedCells.AddUnique(CellCoord);
    FVector CellCenter(CellCoord.X * GridSize + GridSize * 0.5f, 0, CellCoord.Y * GridSize + GridSize * 0.5f);
    UE_LOG(LogTemp, Verbose, TEXT("Loading cell [%d,%d] at (%.0f, %.0f)"), CellCoord.X, CellCoord.Y, CellCenter.X, CellCenter.Z);
}

void AGZWorldPartitionManager::UnloadCell(FIntPoint CellCoord)
{
    LoadedCells.Remove(CellCoord);
    UE_LOG(LogTemp, Verbose, TEXT("Unloading cell [%d,%d]"), CellCoord.X, CellCoord.Y);
}

TArray<FIntPoint> AGZWorldPartitionManager::GetActiveCells() const { return LoadedCells; }
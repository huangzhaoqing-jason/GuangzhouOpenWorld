#include "GZJoltErrorRecovery.h"

UGZJoltErrorRecovery::UGZJoltErrorRecovery()
{
}

void UGZJoltErrorRecovery::RegisterObject(int32 ObjectID, const FVector& Location)
{
	LastKnownLocations.Add(ObjectID, Location);
}

void UGZJoltErrorRecovery::OnObjectError(int32 ObjectID)
{
	UE_LOG(LogTemp, Warning, TEXT("[GZJoltErrorRecovery] Isolating error on object %d"), ObjectID);
	if (Snapshots.Contains(ObjectID))
	{
		ResetObjectToGround(ObjectID, Snapshots[ObjectID].Location);
	}
}

bool UGZJoltErrorRecovery::IsObjectStuck(int32 ObjectID, const FVector& CurrentLocation) const
{
	const FVector* Last = LastKnownLocations.Find(ObjectID);
	if (!Last)
	{
		return false;
	}
	return FVector::DistSquared(*Last, CurrentLocation) < StuckThresholdMeters * StuckThresholdMeters;
}

void UGZJoltErrorRecovery::ResetObjectToGround(int32 ObjectID, const FVector& GroundLocation)
{
	UE_LOG(LogTemp, Log, TEXT("[GZJoltErrorRecovery] Resetting object %d to safe ground location"), ObjectID);
	LastKnownLocations.Add(ObjectID, GroundLocation);
}

void UGZJoltErrorRecovery::SnapshotScene()
{
	for (const auto& Pair : LastKnownLocations)
	{
		FZJoltObjectSnapshot Snapshot;
		Snapshot.ObjectID = Pair.Key;
		Snapshot.Location = Pair.Value;
		Snapshot.bValid = true;
		Snapshots.Add(Pair.Key, Snapshot);
	}
}

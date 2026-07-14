#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZJoltErrorRecovery.generated.h"

USTRUCT(BlueprintType)
struct FZJoltObjectSnapshot
{
	GENERATED_BODY()

	UPROPERTY() int32 ObjectID = 0;
	UPROPERTY() FVector Location;
	UPROPERTY() FVector Velocity;
	UPROPERTY() bool bValid = false;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class JOLTPHYSICS_API UGZJoltErrorRecovery : public UObject
{
	GENERATED_BODY()

public:
	UGZJoltErrorRecovery();

	UFUNCTION(BlueprintCallable, Category="JoltRecovery")
	void RegisterObject(int32 ObjectID, const FVector& Location);

	UFUNCTION(BlueprintCallable, Category="JoltRecovery")
	void OnObjectError(int32 ObjectID);

	UFUNCTION(BlueprintCallable, Category="JoltRecovery")
	bool IsObjectStuck(int32 ObjectID, const FVector& CurrentLocation) const;

	UFUNCTION(BlueprintCallable, Category="JoltRecovery")
	void ResetObjectToGround(int32 ObjectID, const FVector& GroundLocation);

	UFUNCTION(BlueprintCallable, Category="JoltRecovery")
	void SnapshotScene();

private:
	TMap<int32, FZJoltObjectSnapshot> Snapshots;
	TMap<int32, FVector> LastKnownLocations;
	float StuckThresholdMeters = 0.1f;
	float StuckTimeSeconds = 2.0f;
};

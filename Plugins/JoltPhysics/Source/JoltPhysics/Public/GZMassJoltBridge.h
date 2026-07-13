#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZMassJoltBridge.generated.h"

USTRUCT(BlueprintType)
struct FZMassAgentPhysicsData
{
	GENERATED_BODY()

	UPROPERTY() int32 EntityIndex = 0;
	UPROPERTY() FVector Location;
	UPROPERTY() FVector Velocity;
	UPROPERTY() float CapsuleRadius = 35.0f;
	UPROPERTY() float CapsuleHalfHeight = 90.0f;
	UPROPERTY() bool bActive = false;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class JOLTPHYSICS_API UGZMassJoltBridge : public UObject
{
	GENERATED_BODY()

public:
	UGZMassJoltBridge();

	UFUNCTION(BlueprintCallable, Category="MassJolt")
	void Initialize(int32 MaxAgents);

	UFUNCTION(BlueprintCallable, Category="MassJolt")
	void SyncAgents(const TArray<FZMassAgentPhysicsData>& Agents);

	UFUNCTION(BlueprintCallable, Category="MassJolt")
	void UpdateLOD(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category="MassJolt")
	void SetPlayerDistance(int32 EntityIndex, float DistanceMeters);

	UFUNCTION(BlueprintCallable, Category="MassJolt")
	bool RunBatchCollisionChecks(TArray<int32>& OutCollidingAgents);

	UFUNCTION(BlueprintCallable, Category="MassJolt")
	float GetLastUpdateTimeMs() const { return LastUpdateTimeMs; }

private:
	TArray<FZMassAgentPhysicsData> ActiveAgents;
	TMap<int32, float> AgentDistances;
	float LastUpdateTimeMs = 0.0f;
	int32 MaxSupportedAgents = 12000;

	void InternalBatchUpdate(const TArray<FZMassAgentPhysicsData>& Agents);
};

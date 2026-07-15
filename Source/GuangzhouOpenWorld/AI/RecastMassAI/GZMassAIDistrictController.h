#pragma once

// GZMassAIDistrictController.h
// 分区块分布式 AI 调度控制器：替代全局集中调度，按玩家所在 District 分配 AI 预算

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Game/GZGameMode.h"
#include "GZMassAIDistrictController.generated.h"

USTRUCT()
struct FDistrictAIBudget
{
	GENERATED_BODY()

	UPROPERTY() EGZCityDistrict District = EGZCityDistrict::TianheCBD;
	UPROPERTY() int32 MaxAgents = 256;
	UPROPERTY() int32 CurrentAgents = 0;
	UPROPERTY() float UpdateInterval = 0.5f;
	UPROPERTY() float LODDistance = 10000.0f;
};

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZMassAIDistrictController : public UObject
{
	GENERATED_BODY()

public:
	UGZMassAIDistrictController();

	UFUNCTION(BlueprintCallable)
	void Initialize();

	UFUNCTION(BlueprintCallable)
	void TickDistrictAI(float DeltaTime, EGZCityDistrict PlayerDistrict, FVector PlayerLocation);

	UFUNCTION(BlueprintCallable)
	void SetDistrictBudget(EGZCityDistrict District, int32 MaxAgents, float UpdateInterval, float LODDistance);

	UFUNCTION(BlueprintPure)
	const TArray<FDistrictAIBudget>& GetDistrictBudgets() const { return DistrictBudgets; }

	UFUNCTION(BlueprintPure)
	int32 GetActiveAgentCount() const { return ActiveAgentCount; }

private:
	void DistributeBudgets(EGZCityDistrict PlayerDistrict);
	void UpdateAgentsParallel(float DeltaTime, FVector PlayerLocation);

	UPROPERTY()
	TArray<FDistrictAIBudget> DistrictBudgets;

	UPROPERTY()
	EGZCityDistrict CurrentPlayerDistrict = EGZCityDistrict::TianheCBD;

	float TickAccumulator = 0.0f;
	int32 ActiveAgentCount = 0;
};

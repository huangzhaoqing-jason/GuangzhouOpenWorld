#include "AI/RecastMassAI/GZMassAIDistrictController.h"
#include "Core/Platform/GZGCDDispatch.h"
#include "GuangzhouOpenWorld.h"

UGZMassAIDistrictController::UGZMassAIDistrictController()
{
}

void UGZMassAIDistrictController::Initialize()
{
	DistrictBudgets.Empty();

	// 默认各区 AI 预算：玩家所在区高密度，相邻区中密度，其余低密度
	SetDistrictBudget(EGZCityDistrict::TianheCBD,        512, 0.25f, 15000.0f);
	SetDistrictBudget(EGZCityDistrict::YuexiuOldTown,    384, 0.33f, 12000.0f);
	SetDistrictBudget(EGZCityDistrict::LiwanXiguan,      256, 0.50f, 10000.0f);
	SetDistrictBudget(EGZCityDistrict::HaizhuInnovation, 320, 0.40f, 12000.0f);
	SetDistrictBudget(EGZCityDistrict::BaiyunSuburban,   128, 1.00f, 8000.0f);
	SetDistrictBudget(EGZCityDistrict::PanyuUniversity,  192, 0.50f, 9000.0f);
	SetDistrictBudget(EGZCityDistrict::HuangpuPort,      160, 0.66f, 9000.0f);
	SetDistrictBudget(EGZCityDistrict::NanshaNewArea,    96,  1.00f, 7000.0f);
	SetDistrictBudget(EGZCityDistrict::ZhujiangRiver,    64,  1.00f, 6000.0f);

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("MassAI District Controller initialized with %d districts"), DistrictBudgets.Num());
}

void UGZMassAIDistrictController::TickDistrictAI(float DeltaTime, EGZCityDistrict PlayerDistrict, FVector PlayerLocation)
{
	if (PlayerDistrict != CurrentPlayerDistrict)
	{
		CurrentPlayerDistrict = PlayerDistrict;
		DistributeBudgets(PlayerDistrict);
	}

	TickAccumulator += DeltaTime;
	if (TickAccumulator < 0.05f) return;
	TickAccumulator = 0.0f;

	UpdateAgentsParallel(DeltaTime, PlayerLocation);
}

void UGZMassAIDistrictController::SetDistrictBudget(EGZCityDistrict District, int32 MaxAgents, float UpdateInterval, float LODDistance)
{
	for (FDistrictAIBudget& Budget : DistrictBudgets)
	{
		if (Budget.District == District)
		{
			Budget.MaxAgents = MaxAgents;
			Budget.UpdateInterval = UpdateInterval;
			Budget.LODDistance = LODDistance;
			return;
		}
	}

	FDistrictAIBudget NewBudget;
	NewBudget.District = District;
	NewBudget.MaxAgents = MaxAgents;
	NewBudget.UpdateInterval = UpdateInterval;
	NewBudget.LODDistance = LODDistance;
	DistrictBudgets.Add(NewBudget);
}

void UGZMassAIDistrictController::DistributeBudgets(EGZCityDistrict PlayerDistrict)
{
	// 简单策略：玩家所在区保持满预算，其他区减半
	for (FDistrictAIBudget& Budget : DistrictBudgets)
	{
		if (Budget.District == PlayerDistrict)
		{
			Budget.CurrentAgents = FMath::Min(Budget.CurrentAgents + 32, Budget.MaxAgents);
		}
		else
		{
			Budget.CurrentAgents = FMath::Max(Budget.CurrentAgents - 32, Budget.MaxAgents / 2);
		}
	}

	UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("MassAI budgets redistributed for district %d"), (int32)PlayerDistrict);
}

void UGZMassAIDistrictController::UpdateAgentsParallel(float DeltaTime, FVector PlayerLocation)
{
	ActiveAgentCount = 0;
	for (const FDistrictAIBudget& Budget : DistrictBudgets)
	{
		ActiveAgentCount += Budget.CurrentAgents;
	}

	if (ActiveAgentCount <= 0) return;

	// 使用 GCD 并行更新所有 agent 的 LOD/状态
	GZGCD::ParallelFor(ActiveAgentCount, [this, DeltaTime, PlayerLocation](int32 Index)
	{
		// 占位：实际应更新 MassEntity 或 Recast agent 状态
		// 这里仅验证分片并行可行性，避免修改未接入的 MassAI 系统
		(void)DeltaTime;
		(void)PlayerLocation;
		(void)Index;
	});
}

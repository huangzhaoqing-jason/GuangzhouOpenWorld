#include "Game/GZMissionManager.h"
#include "Game/GZMissionJsonLoader.h"
#include "GuangzhouOpenWorld.h"

UGZMissionManager::UGZMissionManager()
{
}

void UGZMissionManager::StartMission(const FName& MissionID)
{
	FGZMissionData* Mission = GetMission(MissionID);
	if (Mission && Mission->Status == EGZMissionStatus::Available)
	{
		Mission->Status = EGZMissionStatus::Active;
		OnMissionStarted.Broadcast(MissionID);
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Mission started: %s"), *MissionID.ToString());
	}
}

void UGZMissionManager::CompleteMission(const FName& MissionID)
{
	FGZMissionData* Mission = GetMission(MissionID);
	if (Mission && Mission->Status == EGZMissionStatus::Active)
	{
		Mission->Status = EGZMissionStatus::Completed;
		OnMissionCompleted.Broadcast(MissionID, true);
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Mission completed: %s"), *MissionID.ToString());
	}
}

void UGZMissionManager::FailMission(const FName& MissionID)
{
	FGZMissionData* Mission = GetMission(MissionID);
	if (Mission && Mission->Status == EGZMissionStatus::Active)
	{
		Mission->Status = EGZMissionStatus::Failed;
		OnMissionCompleted.Broadcast(MissionID, false);
	}
}

void UGZMissionManager::UpdateObjectiveProgress(const FName& MissionID, int32 ObjectiveIndex, int32 Progress)
{
	FGZMissionData* Mission = GetMission(MissionID);
	if (Mission && ObjectiveIndex >= 0 && ObjectiveIndex < Mission->Objectives.Num())
	{
		Mission->Objectives[ObjectiveIndex].CurrentCount += Progress;
		if (Mission->Objectives[ObjectiveIndex].CurrentCount >= Mission->Objectives[ObjectiveIndex].TargetCount)
		{
			Mission->Objectives[ObjectiveIndex].bIsCompleted = true;
		}
		OnMissionUpdated.Broadcast(MissionID);
	}
}

FGZMissionData* UGZMissionManager::GetMission(const FName& MissionID)
{
	for (FGZMissionData& M : MissionRegistry)
	{
		if (M.MissionID == MissionID) return &M;
	}
	return nullptr;
}

TArray<FGZMissionData> UGZMissionManager::GetActiveMissions() const
{
	TArray<FGZMissionData> Active;
	for (const FGZMissionData& M : MissionRegistry)
	{
		if (M.Status == EGZMissionStatus::Active) Active.Add(M);
	}
	return Active;
}

TArray<FGZMissionData> UGZMissionManager::GetAvailableMissions() const
{
	TArray<FGZMissionData> Available;
	for (const FGZMissionData& M : MissionRegistry)
	{
		if (M.Status == EGZMissionStatus::Available) Available.Add(M);
	}
	return Available;
}

float UGZMissionManager::GetMissionProgress(const FName& MissionID) const
{
	for (const FGZMissionData& M : MissionRegistry)
	{
		if (M.MissionID == MissionID && M.Objectives.Num() > 0)
		{
			int32 Completed = 0;
			for (const FGZMissionObjective& Obj : M.Objectives)
			{
				if (Obj.bIsCompleted) Completed++;
			}
			return (float)Completed / M.Objectives.Num();
		}
	}
	return 0.0f;
}

void UGZMissionManager::RegisterMission(const FGZMissionData& Mission)
{
	MissionRegistry.Add(Mission);
}

int32 UGZMissionManager::LoadAct1MissionsFromJson()
{
	return FGZMissionJsonLoader::LoadAct1Into(*this);
}
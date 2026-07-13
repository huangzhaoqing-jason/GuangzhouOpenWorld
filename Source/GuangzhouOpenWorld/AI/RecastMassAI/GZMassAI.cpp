#include "AI/RecastMassAI/GZMassAI.h"
#include "GuangzhouOpenWorld.h"
#include "Math/UnrealMathUtility.h"

FGZNPCSchedule UGZMassAI::GetScheduleForHour(float Hour)
{
	FGZNPCSchedule Schedule;
	Schedule.HourOfDay = Hour;

	if (Hour >= 0.0f && Hour < 6.0f)
	{
		Schedule.RestProbability = 1.0f;
		Schedule.CommuteProbability = 0.0f;
		Schedule.WorkProbability = 0.0f;
		Schedule.ShopProbability = 0.0f;
	}
	else if (Hour >= 6.0f && Hour < 9.0f)
	{
		Schedule.CommuteProbability = 1.0f;
		Schedule.WorkProbability = 0.0f;
		Schedule.ShopProbability = 0.0f;
		Schedule.RestProbability = 0.0f;
	}
	else if (Hour >= 9.0f && Hour < 18.0f)
	{
		Schedule.WorkProbability = 0.6f;
		Schedule.CommuteProbability = 0.1f;
		Schedule.ShopProbability = 0.2f;
		Schedule.RestProbability = 0.1f;
	}
	else if (Hour >= 18.0f && Hour < 21.0f)
	{
		Schedule.ShopProbability = 0.8f;
		Schedule.CommuteProbability = 0.1f;
		Schedule.WorkProbability = 0.05f;
		Schedule.RestProbability = 0.05f;
	}
	else
	{
		Schedule.ShopProbability = 0.2f;
		Schedule.CommuteProbability = 0.1f;
		Schedule.WorkProbability = 0.0f;
		Schedule.RestProbability = 0.7f;
	}

	return Schedule;
}

FGZPoliceTactics UGZMassAI::GetPoliceTacticsForWantedLevel(int32 WantedLevel)
{
	FGZPoliceTactics Tactics;
	Tactics.WantedLevel = FMath::Clamp(WantedLevel, 0, 5);

	switch (WantedLevel)
	{
	case 1:
		Tactics.OfficerCount = 2;
		Tactics.bUseFlanking = false;
		Tactics.bUseCover = false;
		Tactics.bUsePredictivePathfinding = false;
		Tactics.bSurround = false;
		break;
	case 2:
		Tactics.OfficerCount = 4;
		Tactics.bUseFlanking = false;
		Tactics.bUseCover = false;
		Tactics.bUsePredictivePathfinding = false;
		Tactics.bSurround = false;
		break;
	case 3:
		Tactics.OfficerCount = 6;
		Tactics.bUseFlanking = true;
		Tactics.bUseCover = false;
		Tactics.bUsePredictivePathfinding = false;
		Tactics.bSurround = false;
		break;
	case 4:
		Tactics.OfficerCount = 8;
		Tactics.bUseFlanking = true;
		Tactics.bUseCover = true;
		Tactics.bUsePredictivePathfinding = false;
		Tactics.bSurround = false;
		break;
	case 5:
		Tactics.OfficerCount = 12;
		Tactics.bUseFlanking = true;
		Tactics.bUseCover = true;
		Tactics.bUsePredictivePathfinding = true;
		Tactics.bSurround = true;
		break;
	}

	return Tactics;
}

UGZMassAI::UGZMassAI()
{
}

void UGZMassAI::Initialize()
{
	Agents.Empty();
	AgentLODs.Empty();
	PolicePositions.Empty();
	AccidentLocations.Empty();
	NextAgentID = 0;

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Mass AI: max agents=%d, LOD zones: <100m=%.0fHz, 100-200m=%.0fHz, >200m=%.0fHz, driver ratio %.0f:%.0f:%.0f"),
		MaxAgents, NearTickRate, MediumTickRate, FarTickRate, DriverRatioAggressive, DriverRatioCautious, DriverRatioLawful);
}

void UGZMassAI::Tick(float DeltaTime, const FVector& PlayerLocation)
{
	AgentLODCheckTimer += DeltaTime;
	if (AgentLODCheckTimer >= AgentLODCheckInterval)
	{
		AgentLODCheckTimer = 0.0f;
		AssignAgentLODs(PlayerLocation);
	}

	ScheduleUpdateTimer += DeltaTime;
	if (ScheduleUpdateTimer >= ScheduleUpdateInterval)
	{
		ScheduleUpdateTimer = 0.0f;
		float Hour = FMath::Fmod(DeltaTime * 0.1f, 24.0f);
		UpdateAgentSchedules(DeltaTime, Hour);
	}

	PoliceUpdateTimer += DeltaTime;
	if (PoliceUpdateTimer >= PoliceUpdateInterval)
	{
		PoliceUpdateTimer = 0.0f;
		UpdatePoliceBehavior(DeltaTime, PlayerLocation);
	}

	UpdateDriverBehavior(DeltaTime);
}

void UGZMassAI::RegisterAgent(int32 AgentID, const FGZAgentIdentity& Identity)
{
	if (Agents.Num() >= MaxAgents) return;

	Agents.Add(AgentID, Identity);
	AgentLODs.Add(AgentID, EGZAgentLOD::Far);
}

void UGZMassAI::UnregisterAgent(int32 AgentID)
{
	Agents.Remove(AgentID);
	AgentLODs.Remove(AgentID);
}

void UGZMassAI::UpdateAgentLOD(const FVector& AgentLocation, const FVector& PlayerLocation, int32 AgentID)
{
	float Dist = FVector::Dist(AgentLocation, PlayerLocation);
	EGZAgentLOD NewLOD;

	if (Dist < NearDistance)
	{
		NewLOD = EGZAgentLOD::Near;
	}
	else if (Dist < MediumDistance)
	{
		NewLOD = EGZAgentLOD::Medium;
	}
	else
	{
		NewLOD = EGZAgentLOD::Far;
	}

	if (AgentLODs.Contains(AgentID))
	{
		AgentLODs[AgentID] = NewLOD;
	}
}

void UGZMassAI::UpdatePoliceResponse(int32 WantedLevel, const FVector& TargetLocation)
{
	CurrentTactics = GetPoliceTacticsForWantedLevel(WantedLevel);

	PolicePositions.Empty();
	for (int32 i = 0; i < CurrentTactics.OfficerCount; ++i)
	{
		float AngleOffset = (360.0f / CurrentTactics.OfficerCount) * i;
		float Dist = 500.0f + i * 200.0f;
		FVector Offset = FVector(
			FMath::Cos(FMath::DegreesToRadians(AngleOffset)) * Dist,
			FMath::Sin(FMath::DegreesToRadians(AngleOffset)) * Dist,
			0.0f
		);

		if (CurrentTactics.bSurround)
		{
			Dist = 300.0f + i * 100.0f;
			Offset = FVector(
				FMath::Cos(FMath::DegreesToRadians(AngleOffset)) * Dist,
				FMath::Sin(FMath::DegreesToRadians(AngleOffset)) * Dist,
				0.0f
			);
		}

		PolicePositions.Add(TargetLocation + Offset);
	}
}

EGZAgentLOD UGZMassAI::GetAgentLOD(int32 AgentID) const
{
	if (const EGZAgentLOD* LOD = AgentLODs.Find(AgentID))
	{
		return *LOD;
	}
	return EGZAgentLOD::Far;
}

void UGZMassAI::UpdateAgentSchedules(float DeltaTime, float HourOfDay)
{
	FGZNPCSchedule Schedule = GetScheduleForHour(HourOfDay);

	for (auto& Pair : Agents)
	{
		FGZAgentIdentity& Agent = Pair.Value;
		Agent.ScheduleTime = HourOfDay;

		float Rand = FMath::FRand();
		if (Rand < Schedule.CommuteProbability)
		{
			Agent.EmotionalState = EGZEmotionalState::Neutral;
		}
		else if (Rand < Schedule.CommuteProbability + Schedule.WorkProbability)
		{
			Agent.EmotionalState = EGZEmotionalState::Neutral;
		}
		else if (Rand < Schedule.CommuteProbability + Schedule.WorkProbability + Schedule.ShopProbability)
		{
			Agent.EmotionalState = Agent.EmotionalState == EGZEmotionalState::Anxious ? EGZEmotionalState::Happy : EGZEmotionalState::Neutral;
		}
	}
}

void UGZMassAI::UpdateDriverBehavior(float DeltaTime)
{
	for (auto& Pair : Agents)
	{
		FGZAgentIdentity& Agent = Pair.Value;
		if (Agent.Occupation != EGZOccupation::Driver) continue;

		float SpeedMult = 1.0f;
		switch (Agent.DriverType)
		{
		case EGZDriverType::Aggressive:
			SpeedMult = AggressiveSpeedMultiplier;
			break;
		case EGZDriverType::Cautious:
			SpeedMult = CautiousSpeedMultiplier;
			break;
		case EGZDriverType::Lawful:
			SpeedMult = LawfulSpeedMultiplier;
			break;
		}
	}
}

void UGZMassAI::UpdatePoliceBehavior(float DeltaTime, const FVector& PlayerLocation)
{
	if (CurrentTactics.WantedLevel <= 0) return;

	for (int32 i = 0; i < PolicePositions.Num(); ++i)
	{
		FVector Direction = (PlayerLocation - PolicePositions[i]).GetSafeNormal();
		float Speed = 300.0f;

		if (CurrentTactics.bUseFlanking)
		{
			FVector PerpDir = FVector(-Direction.Y, Direction.X, 0.0f);
			PolicePositions[i] += (Direction + PerpDir * 0.3f).GetSafeNormal() * Speed * DeltaTime;
		}
		else if (CurrentTactics.bUsePredictivePathfinding)
		{
			FVector PredictedPos = PlayerLocation + Direction * 500.0f;
			Direction = (PredictedPos - PolicePositions[i]).GetSafeNormal();
			PolicePositions[i] += Direction * Speed * 1.5f * DeltaTime;
		}
		else
		{
			PolicePositions[i] += Direction * Speed * DeltaTime;
		}

		if (CurrentTactics.bSurround)
		{
			float Dist = FVector::Dist(PolicePositions[i], PlayerLocation);
			if (Dist < 200.0f)
			{
				FVector Away = (PolicePositions[i] - PlayerLocation).GetSafeNormal();
				PolicePositions[i] += Away * 50.0f * DeltaTime;
			}
		}
	}
}

void UGZMassAI::AssignAgentLODs(const FVector& PlayerLocation)
{
	for (auto& Pair : Agents)
	{
		FVector AgentPos = FVector::ZeroVector;
		UpdateAgentLOD(AgentPos, PlayerLocation, Pair.Key);
	}
}

float UGZMassAI::GetAgentLODTickRate(EGZAgentLOD LOD) const
{
	switch (LOD)
	{
	case EGZAgentLOD::Near: return 1.0f / NearTickRate;
	case EGZAgentLOD::Medium: return 1.0f / MediumTickRate;
	case EGZAgentLOD::Far: return 1.0f / FarTickRate;
	default: return 1.0f / FarTickRate;
	}
}

void UGZMassAI::AssignDriverTypes()
{
	float TotalRatio = DriverRatioAggressive + DriverRatioCautious + DriverRatioLawful;
	float AggressiveThreshold = DriverRatioAggressive / TotalRatio;
	float CautiousThreshold = (DriverRatioAggressive + DriverRatioCautious) / TotalRatio;

	for (auto& Pair : Agents)
	{
		FGZAgentIdentity& Agent = Pair.Value;
		if (Agent.Occupation != EGZOccupation::Driver) continue;

		float Rand = FMath::FRand();
		if (Rand < AggressiveThreshold)
		{
			Agent.DriverType = EGZDriverType::Aggressive;
		}
		else if (Rand < CautiousThreshold)
		{
			Agent.DriverType = EGZDriverType::Cautious;
		}
		else
		{
			Agent.DriverType = EGZDriverType::Lawful;
		}
	}
}

void UGZMassAI::HandleAccidentReroute(int32 AgentID, const FVector& AccidentLocation)
{
	AccidentLocations.Add(AccidentLocation);

	if (Agents.Contains(AgentID))
	{
		EGZAgentLOD LOD = GetAgentLOD(AgentID);

		if (LOD == EGZAgentLOD::Near)
		{
			float RerouteDelay = AccidentRerouteTime;
			UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("Accident reroute for agent %d, delay=%.2fs"), AgentID, RerouteDelay);
		}
	}
}
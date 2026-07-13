#include "AI/RecastMassAI/GZMassAI.h"
#include "AI/RecastMassAI/GZNavigationSystem.h"
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
	RushHourDensityMultiplier = 1.0f;
	NightSparseDensityMultiplier = 0.2f;

	AreaDensities.Add(EGZAreaType::Commercial, 1.0f);
	AreaDensities.Add(EGZAreaType::Residential, 0.7f);
	AreaDensities.Add(EGZAreaType::Industrial, 0.5f);
	AreaDensities.Add(EGZAreaType::Park, 0.3f);
	AreaDensities.Add(EGZAreaType::Transport, 0.8f);
	AreaDensities.Add(EGZAreaType::Tourist, 0.9f);

	WeatherActions.SetNum(5);
	WeatherActions[0].WeatherType = EGZWeatherType::Clear;
	WeatherActions[0].SpeedMultiplier = 1.0f;

	WeatherActions[1].WeatherType = EGZWeatherType::Cloudy;
	WeatherActions[1].SpeedMultiplier = 1.0f;

	WeatherActions[2].WeatherType = EGZWeatherType::Rain;
	WeatherActions[2].bHasUmbrella = true;
	WeatherActions[2].bCoversHead = true;
	WeatherActions[2].bPrefersCoveredPaths = true;
	WeatherActions[2].SpeedMultiplier = 1.0f;

	WeatherActions[3].WeatherType = EGZWeatherType::Storm;
	WeatherActions[3].bStaysIndoor = true;
	WeatherActions[3].SpeedMultiplier = 0.5f;

	WeatherActions[4].WeatherType = EGZWeatherType::FogHaze;
	WeatherActions[4].bPrefersCoveredPaths = true;
	WeatherActions[4].SpeedMultiplier = 0.55f;

	DialogUpdateTimer = 0.0f;
	DialogUpdateInterval = 3.0f;
	MemoryUpdateTimer = 0.0f;
	MemoryUpdateInterval = 5.0f;
}

void UGZMassAI::Initialize()
{
	Agents.Empty();
	AgentLODs.Empty();
	AgentWeatherStates.Empty();
	PolicePositions.Empty();
	AccidentLocations.Empty();
	NextAgentID = 0;
	CurrentWeather = EGZWeatherType::Clear;
	CurrentHourOfDay = 12.0f;

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Mass AI: max agents=%d, LOD zones: <100m=%.0fHz, 100-200m=%.0fHz, >200m=%.0fHz, driver ratio %.0f:%.0f:%.0f, weather response active"),
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
		CurrentHourOfDay = Hour;
		UpdateAgentSchedules(DeltaTime, Hour);
	}

	WeatherUpdateTimer += DeltaTime;
	if (WeatherUpdateTimer >= WeatherUpdateInterval)
	{
		WeatherUpdateTimer = 0.0f;
		UpdateAgentWeatherStates(DeltaTime);
	}

	PoliceUpdateTimer += DeltaTime;
	if (PoliceUpdateTimer >= PoliceUpdateInterval)
	{
		PoliceUpdateTimer = 0.0f;
		UpdatePoliceBehavior(DeltaTime, PlayerLocation);
	}

	UpdateDriverBehavior(DeltaTime);

	DialogUpdateTimer += DeltaTime;
	if (DialogUpdateTimer >= DialogUpdateInterval)
	{
		DialogUpdateTimer = 0.0f;
		UpdateNPCInteractions(DeltaTime, CurrentHourOfDay);
	}

	MemoryUpdateTimer += DeltaTime;
	if (MemoryUpdateTimer >= MemoryUpdateInterval)
	{
		MemoryUpdateTimer = 0.0f;
		UpdateAgentMemory(DeltaTime, PlayerLocation);
	}

	UpdateRushHour(CurrentHourOfDay);
	ApplyWeatherActions(CurrentWeather);
}

void UGZMassAI::RegisterAgent(int32 AgentID, const FGZAgentIdentity& Identity)
{
	if (Agents.Num() >= MaxAgents) return;

	Agents.Add(AgentID, Identity);
	AgentLODs.Add(AgentID, EGZAgentLOD::Far);

	FAgentWeatherState DefaultWeatherState;
	DefaultWeatherState.CurrentResponse = ENPCWeatherResponse::Normal;
	DefaultWeatherState.SpeedMultiplier = 1.0f;
	DefaultWeatherState.bHasReachedShelter = false;
	DefaultWeatherState.WeatherResponseTimer = 0.0f;
	AgentWeatherStates.Add(AgentID, DefaultWeatherState);
}

void UGZMassAI::UnregisterAgent(int32 AgentID)
{
	Agents.Remove(AgentID);
	AgentLODs.Remove(AgentID);
	AgentWeatherStates.Remove(AgentID);
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

	if (Agents.Contains(AgentID))
	{
		Agents[AgentID].CurrentLocation = AgentLocation;
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

ENPCWeatherResponse UGZMassAI::GetAgentWeatherResponse(int32 AgentID) const
{
	if (const FAgentWeatherState* State = AgentWeatherStates.Find(AgentID))
	{
		return State->CurrentResponse;
	}
	return ENPCWeatherResponse::Normal;
}

void UGZMassAI::UpdateWeatherResponse(EGZWeatherType Weather, float HourOfDay)
{
	CurrentWeather = Weather;
	CurrentHourOfDay = HourOfDay;

	switch (Weather)
	{
	case EGZWeatherType::Rain:
		ApplyRainShelter();
		break;
	case EGZWeatherType::FogHaze:
		ApplyFogSlowdown();
		break;
	case EGZWeatherType::Storm:
		ApplyStormResponse();
		break;
	case EGZWeatherType::Clear:
	case EGZWeatherType::Cloudy:
	default:
		for (auto& Pair : AgentWeatherStates)
		{
			FAgentWeatherState& State = Pair.Value;
			State.CurrentResponse = ENPCWeatherResponse::Normal;
			State.SpeedMultiplier = 1.0f;
			State.bHasReachedShelter = false;
			State.ShelterLocation = FVector::ZeroVector;
		}
		break;
	}

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Weather response updated: weather=%d, hour=%.1f, agents=%d"),
		(int32)Weather, HourOfDay, AgentWeatherStates.Num());

	ApplyWeatherActions(Weather);
}

void UGZMassAI::ApplyRainShelter()
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Rain shelter: NPCs seeking covered areas within %.0fm, speed=%.1fx"),
		ShelterSearchRadius * 0.01f, RainShelterSpeedMultiplier);

	for (auto& Pair : Agents)
	{
		int32 AgentID = Pair.Key;
		FGZAgentIdentity& Agent = Pair.Value;

		if (Agent.bIsIndoor)
		{
			if (AgentWeatherStates.Contains(AgentID))
			{
				AgentWeatherStates[AgentID].CurrentResponse = ENPCWeatherResponse::Normal;
				AgentWeatherStates[AgentID].SpeedMultiplier = 1.0f;
				AgentWeatherStates[AgentID].bHasReachedShelter = true;
			}
			continue;
		}

		if (!AgentWeatherStates.Contains(AgentID))
		{
			continue;
		}

		FAgentWeatherState& WeatherState = AgentWeatherStates[AgentID];
		WeatherState.CurrentResponse = ENPCWeatherResponse::SeekShelter;
		WeatherState.SpeedMultiplier = RainShelterSpeedMultiplier;
		WeatherState.bHasReachedShelter = false;

		WeatherState.ShelterLocation = Agent.WorkLocation;
		if (Agent.WorkLocation.IsNearlyZero())
		{
			WeatherState.ShelterLocation = Agent.HomeLocation;
		}
	}
}

void UGZMassAI::ApplyFogSlowdown()
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Fog slowdown: outdoor NPC speed *= %.1f, indoor NPCs unaffected"),
		FogSlowdownMultiplier);

	for (auto& Pair : Agents)
	{
		int32 AgentID = Pair.Key;
		FGZAgentIdentity& Agent = Pair.Value;

		if (Agent.bIsIndoor)
		{
			if (AgentWeatherStates.Contains(AgentID))
			{
				AgentWeatherStates[AgentID].CurrentResponse = ENPCWeatherResponse::Normal;
				AgentWeatherStates[AgentID].SpeedMultiplier = 1.0f;
			}
			continue;
		}

		if (!AgentWeatherStates.Contains(AgentID))
		{
			continue;
		}

		FAgentWeatherState& WeatherState = AgentWeatherStates[AgentID];
		WeatherState.CurrentResponse = ENPCWeatherResponse::SlowDown;
		WeatherState.SpeedMultiplier = FogSlowdownMultiplier;
	}
}

void UGZMassAI::ApplyStormResponse()
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Storm response: outdoor NPCs reduced to %.0f%%, all seek indoor shelter"),
		StormOutdoorFraction * 100.0f);

	ReduceOutdoorDensity(StormOutdoorFraction);

	for (auto& Pair : Agents)
	{
		int32 AgentID = Pair.Key;
		FGZAgentIdentity& Agent = Pair.Value;

		if (Agent.bIsIndoor)
		{
			if (AgentWeatherStates.Contains(AgentID))
			{
				AgentWeatherStates[AgentID].CurrentResponse = ENPCWeatherResponse::StayIndoor;
				AgentWeatherStates[AgentID].SpeedMultiplier = 1.0f;
				AgentWeatherStates[AgentID].bHasReachedShelter = true;
			}
			continue;
		}

		if (!AgentWeatherStates.Contains(AgentID))
		{
			continue;
		}

		FAgentWeatherState& WeatherState = AgentWeatherStates[AgentID];
		WeatherState.CurrentResponse = ENPCWeatherResponse::StayIndoor;
		WeatherState.SpeedMultiplier = RainShelterSpeedMultiplier;
		WeatherState.bHasReachedShelter = false;

		WeatherState.ShelterLocation = Agent.HomeLocation;
		if (Agent.HomeLocation.IsNearlyZero())
		{
			WeatherState.ShelterLocation = Agent.WorkLocation;
		}
	}
}

void UGZMassAI::UpdateAgentSchedules(float DeltaTime, float HourOfDay)
{
	FGZNPCSchedule Schedule = GetScheduleForHour(HourOfDay);

	bool bBadWeather = (CurrentWeather == EGZWeatherType::Rain ||
		CurrentWeather == EGZWeatherType::Storm ||
		CurrentWeather == EGZWeatherType::FogHaze);

	for (auto& Pair : Agents)
	{
		FGZAgentIdentity& Agent = Pair.Value;
		Agent.ScheduleTime = HourOfDay;

		if (bBadWeather && !Agent.bIsIndoor)
		{
			Schedule.CommuteProbability *= 0.3f;
			Schedule.WorkProbability *= 0.5f;
			Schedule.ShopProbability *= 0.2f;
			Schedule.RestProbability = FMath::Clamp(1.0f - Schedule.CommuteProbability -
				Schedule.WorkProbability - Schedule.ShopProbability, 0.0f, 1.0f);
		}

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

void UGZMassAI::UpdateAgentWeatherStates(float DeltaTime)
{
	for (auto& Pair : AgentWeatherStates)
	{
		int32 AgentID = Pair.Key;
		FAgentWeatherState& WeatherState = Pair.Value;

		if (WeatherState.CurrentResponse == ENPCWeatherResponse::Normal)
		{
			continue;
		}

		WeatherState.WeatherResponseTimer += DeltaTime;

		if (WeatherState.CurrentResponse == ENPCWeatherResponse::SeekShelter)
		{
			if (!Agents.Contains(AgentID))
			{
				continue;
			}

			FGZAgentIdentity& Agent = Agents[AgentID];
			if (Agent.bIsIndoor)
			{
				WeatherState.bHasReachedShelter = true;
				WeatherState.CurrentResponse = ENPCWeatherResponse::Normal;
				WeatherState.SpeedMultiplier = 1.0f;
				continue;
			}

			float DistToShelter = FVector::Dist(Agent.CurrentLocation, WeatherState.ShelterLocation);
			if (DistToShelter < 200.0f)
			{
				WeatherState.bHasReachedShelter = true;
				WeatherState.CurrentResponse = ENPCWeatherResponse::Normal;
				WeatherState.SpeedMultiplier = 1.0f;
				Agent.bIsIndoor = true;
			}
		}
		else if (WeatherState.CurrentResponse == ENPCWeatherResponse::StayIndoor)
		{
			if (!Agents.Contains(AgentID))
			{
				continue;
			}

			FGZAgentIdentity& Agent = Agents[AgentID];
			if (Agent.bIsIndoor)
			{
				WeatherState.bHasReachedShelter = true;
				continue;
			}

			float DistToShelter = FVector::Dist(Agent.CurrentLocation, WeatherState.ShelterLocation);
			if (DistToShelter < 200.0f)
			{
				WeatherState.bHasReachedShelter = true;
				Agent.bIsIndoor = true;
			}
		}
	}
}

void UGZMassAI::ReduceOutdoorDensity(float OutdoorFraction)
{
	TArray<int32> OutdoorAgents;
	for (auto& Pair : Agents)
	{
		if (!Pair.Value.bIsIndoor)
		{
			OutdoorAgents.Add(Pair.Key);
		}
	}

	int32 KeepCount = FMath::Max(1, FMath::RoundToInt(OutdoorAgents.Num() * OutdoorFraction));
	int32 RemoveCount = OutdoorAgents.Num() - KeepCount;

	for (int32 i = 0; i < RemoveCount; ++i)
	{
		int32 Idx = FMath::RandRange(0, OutdoorAgents.Num() - 1);
		int32 AgentID = OutdoorAgents[Idx];

		if (Agents.Contains(AgentID))
		{
			Agents[AgentID].bIsIndoor = true;
		}

		if (AgentWeatherStates.Contains(AgentID))
		{
			AgentWeatherStates[AgentID].CurrentResponse = ENPCWeatherResponse::StayIndoor;
			AgentWeatherStates[AgentID].bHasReachedShelter = true;
		}

		OutdoorAgents.RemoveAt(Idx);
	}

	UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("Storm: kept %d outdoor agents, moved %d indoors"),
		KeepCount, RemoveCount);
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

		if (AgentWeatherStates.Contains(Pair.Key))
		{
			SpeedMult *= AgentWeatherStates[Pair.Key].SpeedMultiplier;
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
		FVector AgentPos = Pair.Value.CurrentLocation;
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

void UGZMassAI::UpdateNPCInteractions(float DeltaTime, float HourOfDay)
{
	static constexpr float InteractionRadius = 500.0f;
	static constexpr float DialogChance = 0.15f;

	for (int32 i = ActiveDialogs.Num() - 1; i >= 0; --i)
	{
		FInterNPCDialog& Dialog = ActiveDialogs[i];
		Dialog.Duration -= DeltaTime;

		if (Dialog.Duration <= 0.0f)
		{
			if (Agents.Contains(Dialog.SpeakerA_ID))
			{
				Agents[Dialog.SpeakerA_ID].bIsInConversation = false;
				Agents[Dialog.SpeakerA_ID].ConversationTargetID = -1;
				Agents[Dialog.SpeakerA_ID].Memory.NPCInteractionCount++;
			}
			if (Agents.Contains(Dialog.SpeakerB_ID))
			{
				Agents[Dialog.SpeakerB_ID].bIsInConversation = false;
				Agents[Dialog.SpeakerB_ID].ConversationTargetID = -1;
				Agents[Dialog.SpeakerB_ID].Memory.NPCInteractionCount++;
			}
			ActiveDialogs.RemoveAt(i);
		}
	}

	TArray<int32> AgentIDs;
	Agents.GetKeys(AgentIDs);

	for (int32 i = 0; i < AgentIDs.Num(); ++i)
	{
		int32 AgentA = AgentIDs[i];
		if (!Agents.Contains(AgentA)) continue;
		FGZAgentIdentity& IdentityA = Agents[AgentA];
		if (IdentityA.bIsInConversation) continue;

		for (int32 j = i + 1; j < AgentIDs.Num(); ++j)
		{
			int32 AgentB = AgentIDs[j];
			if (!Agents.Contains(AgentB)) continue;
			FGZAgentIdentity& IdentityB = Agents[AgentB];
			if (IdentityB.bIsInConversation) continue;

			float Dist = FVector::Dist(IdentityA.CurrentLocation, IdentityB.CurrentLocation);
			if (Dist < InteractionRadius && FMath::FRand() < DialogChance)
			{
				GenerateInterNPCDialog(AgentA, AgentB);
				break;
			}
		}
	}
}

void UGZMassAI::UpdateAgentMemory(float DeltaTime, const FVector& PlayerLocation)
{
	static constexpr float HostilityDecayRate = 0.5f;
	static constexpr float ReputationRecoveryRate = 0.2f;
	static constexpr float PlayerInteractionRadius = 800.0f;

	for (auto& Pair : Agents)
	{
		FGZAgentIdentity& Agent = Pair.Value;
		FAgentMemory& Memory = Agent.Memory;

		float DistToPlayer = FVector::Dist(Agent.CurrentLocation, PlayerLocation);
		if (DistToPlayer < PlayerInteractionRadius)
		{
			Memory.PlayerLastInteraction = PlayerLocation;
			Memory.LastInteractionTime = 0.0f;
		}

		Memory.LastInteractionTime += DeltaTime;

		if (Memory.bIsHostile && Memory.LastInteractionTime > 30.0f)
		{
			Memory.bIsHostile = false;
			Memory.ReputationScore = FMath::Min(100.0f, Memory.ReputationScore + ReputationRecoveryRate * DeltaTime);
		}

		if (!Memory.bIsHostile)
		{
			Memory.ReputationScore = FMath::Min(100.0f, Memory.ReputationScore + ReputationRecoveryRate * 0.5f * DeltaTime);
		}
	}
}

void UGZMassAI::GenerateInterNPCDialog(int32 AgentA, int32 AgentB)
{
	if (!Agents.Contains(AgentA) || !Agents.Contains(AgentB)) return;

	FInterNPCDialog Dialog;
	Dialog.DialogID = ActiveDialogs.Num() + 1;
	Dialog.SpeakerA_ID = AgentA;
	Dialog.SpeakerB_ID = AgentB;
	Dialog.bIsWeatherRelated = (CurrentWeather == EGZWeatherType::Rain || CurrentWeather == EGZWeatherType::Storm);

	if (Dialog.bIsWeatherRelated)
	{
		if (CurrentWeather == EGZWeatherType::Rain)
		{
			Dialog.DialogText = TEXT("这雨真大啊...");
			Dialog.Duration = FMath::FRandRange(6.0f, 10.0f);
		}
		else if (CurrentWeather == EGZWeatherType::Storm)
		{
			Dialog.DialogText = TEXT("得赶紧找个地方躲躲！");
			Dialog.Duration = FMath::FRandRange(2.0f, 4.0f);
		}
		else
		{
			Dialog.DialogText = TEXT("天气不太好呢。");
			Dialog.Duration = FMath::FRandRange(3.0f, 6.0f);
		}
	}
	else
	{
		const TCHAR* DialogPool[] = {
			TEXT("你好！今天过得怎么样？"),
			TEXT("最近忙什么呢？"),
			TEXT("听说那边新开了家店！"),
			TEXT("你也是这附近的吗？"),
			TEXT("广州的天气真多变啊。"),
		};
		int32 PoolIdx = FMath::RandRange(0, 4);
		Dialog.DialogText = DialogPool[PoolIdx];
		Dialog.Duration = FMath::FRandRange(3.0f, 8.0f);
	}

	Agents[AgentA].bIsInConversation = true;
	Agents[AgentA].ConversationTargetID = AgentB;
	Agents[AgentB].bIsInConversation = true;
	Agents[AgentB].ConversationTargetID = AgentA;

	ActiveDialogs.Add(Dialog);

	UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("NPC Dialog #%d: Agent %d <-> Agent %d, duration=%.1fs, weather=%d"),
		Dialog.DialogID, AgentA, AgentB, Dialog.Duration, (int32)CurrentWeather);
}

void UGZMassAI::ApplyWeatherActions(EGZWeatherType Weather)
{
	switch (Weather)
	{
	case EGZWeatherType::Rain:
	{
		for (auto& Pair : Agents)
		{
			FGZAgentIdentity& Agent = Pair.Value;
			if (Agent.bIsIndoor) continue;

			if (FMath::FRand() < 0.7f)
			{
				Agent.bHasUmbrella = true;
			}

			if (AgentWeatherStates.Contains(Pair.Key))
			{
				AgentWeatherStates[Pair.Key].SpeedMultiplier = 1.0f;
			}
		}
	}
	break;

	case EGZWeatherType::Storm:
	{
		for (auto& Pair : Agents)
		{
			FGZAgentIdentity& Agent = Pair.Value;
			if (Agent.bIsIndoor) continue;

			if (FMath::FRand() < 0.95f)
			{
				Agent.bIsIndoor = true;
				if (AgentWeatherStates.Contains(Pair.Key))
				{
					AgentWeatherStates[Pair.Key].CurrentResponse = ENPCWeatherResponse::StayIndoor;
					AgentWeatherStates[Pair.Key].bHasReachedShelter = true;
					AgentWeatherStates[Pair.Key].SpeedMultiplier = 0.5f;
				}
			}
		}
	}
	break;

	case EGZWeatherType::FogHaze:
	{
		for (auto& Pair : Agents)
		{
			FGZAgentIdentity& Agent = Pair.Value;
			if (Agent.bIsIndoor) continue;

			if (AgentWeatherStates.Contains(Pair.Key))
			{
				AgentWeatherStates[Pair.Key].SpeedMultiplier = 0.55f;
			}
		}
	}
	break;

	case EGZWeatherType::Clear:
	case EGZWeatherType::Cloudy:
	default:
		for (auto& Pair : Agents)
		{
			FGZAgentIdentity& Agent = Pair.Value;
			Agent.bHasUmbrella = false;
		}
		break;
	}
}

FAgentWeatherAction UGZMassAI::GetWeatherActionForAgent(EGZWeatherType Weather, int32 AgentID) const
{
	FAgentWeatherAction Result;
	Result.WeatherType = Weather;

	if (!Agents.Contains(AgentID))
	{
		return Result;
	}

	const FGZAgentIdentity& Agent = Agents[AgentID];

	for (const FAgentWeatherAction& Action : WeatherActions)
	{
		if (Action.WeatherType == Weather)
		{
			Result = Action;
			break;
		}
	}

	if (Agent.bIsIndoor)
	{
		Result.bHasUmbrella = false;
		Result.bCoversHead = false;
		Result.bPrefersCoveredPaths = false;
		Result.bStaysIndoor = false;
		Result.SpeedMultiplier = 1.0f;
	}

	return Result;
}

void UGZMassAI::UpdateCommuteBehavior(float HourOfDay, EGZAreaType Area)
{
	float CommuteBoost = 0.0f;
	bool bIsRushHour = ((HourOfDay >= 7.0f && HourOfDay < 9.0f) || (HourOfDay >= 17.0f && HourOfDay < 19.0f));
	bool bIsNight = (HourOfDay >= 22.0f || HourOfDay < 6.0f);

	if (bIsRushHour)
	{
		if (Area == EGZAreaType::Commercial || Area == EGZAreaType::Transport)
		{
			CommuteBoost = RushHourDensityMultiplier;
		}
	}
	else if (bIsNight)
	{
		CommuteBoost = NightSparseDensityMultiplier;
	}
	else
	{
		if (AreaDensities.Contains(Area))
		{
			CommuteBoost = AreaDensities[Area];
		}
	}

	UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("Commute behavior: area=%d, hour=%.1f, density=%.2f"),
		(int32)Area, HourOfDay, CommuteBoost);
}

void UGZMassAI::SetAreaDensity(EGZAreaType Area, float Density)
{
	if (AreaDensities.Contains(Area))
	{
		AreaDensities[Area] = FMath::Clamp(Density, 0.0f, 1.0f);
	}
	else
	{
		AreaDensities.Add(Area, FMath::Clamp(Density, 0.0f, 1.0f));
	}
}

void UGZMassAI::UpdateRushHour(float HourOfDay)
{
	bool bIsRushHour = ((HourOfDay >= 7.0f && HourOfDay < 9.0f) || (HourOfDay >= 17.0f && HourOfDay < 19.0f));
	bool bIsNight = (HourOfDay >= 22.0f || HourOfDay < 6.0f);

	if (bIsRushHour)
	{
		RushHourDensityMultiplier = 1.0f;
		if (AreaDensities.Contains(EGZAreaType::Commercial))
		{
			AreaDensities[EGZAreaType::Commercial] = 1.0f;
		}
		if (AreaDensities.Contains(EGZAreaType::Transport))
		{
			AreaDensities[EGZAreaType::Transport] = 1.0f;
		}

		for (auto& Pair : AgentWeatherStates)
		{
			Pair.Value.SpeedMultiplier = FMath::Max(Pair.Value.SpeedMultiplier, 1.2f);
		}
	}
	else if (bIsNight)
	{
		NightSparseDensityMultiplier = 0.2f;
		for (auto& AreaPair : AreaDensities)
		{
			AreaDensities[AreaPair.Key] = FMath::Min(AreaPair.Value, 0.2f);
		}
	}
}
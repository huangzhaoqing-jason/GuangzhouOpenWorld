#include "AI/GZMassAI.h"
#include "GuangzhouOpenWorld.h"
#include "Math/UnrealMathUtility.h"

UGZMassAI::UGZMassAI()
{
}

void UGZMassAI::Initialize(int32 InMaxAgents)
{
	MaxAgents = InMaxAgents;
	AgentStates.Empty();
	AgentStates.Reserve(MaxAgents);
	ActiveAccidents.Empty();
	NextAgentID = 0;
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("MassAI initialized: max agents=%d"), MaxAgents);
}

void UGZMassAI::Tick(float DeltaTime, float CurrentHour, EGZWantedLevel WantedLevel, const FVector& PlayerPosition)
{
	UpdateAgentLOD(DeltaTime, PlayerPosition);
	UpdatePedestrianBehavior(DeltaTime, CurrentHour);
	UpdateTrafficBehavior(DeltaTime);
	UpdatePoliceBehavior(DeltaTime, WantedLevel, PlayerPosition);
	UpdateEmergencyBehavior(DeltaTime);
	UpdateTrafficAccidents(DeltaTime);

	CrowdUpdateTimer += DeltaTime;
	if (CrowdUpdateTimer >= 0.5f)
	{
		CrowdUpdateTimer = 0.0f;
		UpdateCrowdDensity(DeltaTime);
	}

	EmotionalUpdateTimer += DeltaTime;
	if (EmotionalUpdateTimer >= 1.0f)
	{
		EmotionalUpdateTimer = 0.0f;
		UpdateEmotionalStates(DeltaTime);
	}
}

void UGZMassAI::SpawnAgent(EGZAgentType Type, const FVector& Location, const FGZAgentIdentity& Identity)
{
	if (AgentStates.Num() >= MaxAgents) return;

	FGZAgentState NewAgent;
	NewAgent.AgentID = NextAgentID++;
	NewAgent.Type = Type;
	NewAgent.Position = Location;
	NewAgent.TargetPosition = Location;
	NewAgent.Identity = Identity;
	NewAgent.bIsActive = true;

	switch (Type)
	{
	case EGZAgentType::Pedestrian:
		NewAgent.MovementSpeed = 1.4f;
		break;
	case EGZAgentType::Vehicle:
		NewAgent.MovementSpeed = 13.8f;
		NewAgent.DriverType = static_cast<EGZDriverType>(FMath::RandRange(0, 2));
		break;
	case EGZAgentType::Police:
		NewAgent.MovementSpeed = 4.0f;
		break;
	case EGZAgentType::Emergency:
		NewAgent.MovementSpeed = 20.0f;
		break;
	}

	AgentStates.Add(NewAgent);
}

void UGZMassAI::RemoveAgent(int32 AgentID)
{
	for (int32 i = AgentStates.Num() - 1; i >= 0; --i)
	{
		if (AgentStates[i].AgentID == AgentID)
		{
			AgentStates.RemoveAt(i);
			return;
		}
	}
}

void UGZMassAI::ReportTrafficAccident(const FVector& Location, float Severity)
{
	FGZTrafficAccident Accident;
	Accident.Location = Location;
	Accident.Severity = Severity;
	Accident.Duration = Severity * 120.0f;
	Accident.ElapsedTime = 0.0f;
	Accident.CongestionRadius = 100.0f + Severity * 200.0f;
	ActiveAccidents.Add(Accident);

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Traffic accident reported at (%.0f, %.0f, %.0f), severity=%.2f"),
		Location.X, Location.Y, Location.Z, Severity);
}

void UGZMassAI::SetWantedLevel(EGZWantedLevel Level)
{
	for (FGZAgentState& Agent : AgentStates)
	{
		if (Agent.Type == EGZAgentType::Police)
		{
			Agent.WantedResponse = Level;
		}
	}
}

void UGZMassAI::UpdatePedestrianBehavior(float DeltaTime, float CurrentHour)
{
	for (FGZAgentState& Agent : AgentStates)
	{
		if (Agent.Type != EGZAgentType::Pedestrian || !Agent.bIsActive) continue;

		FVector Target;
		Agent.HourOfDay = CurrentHour;

		if (CurrentHour >= 8.0f && CurrentHour < 18.0f)
		{
			Target = Agent.Identity.WorkLocation;
		}
		else if (CurrentHour >= 18.0f && CurrentHour < 22.0f)
		{
			Target = Agent.Identity.HomeLocation + FVector(
				FMath::FRandRange(-500.0f, 500.0f),
				FMath::FRandRange(-500.0f, 500.0f),
				0.0f);
		}
		else
		{
			Target = Agent.Identity.HomeLocation;
		}

		Agent.TargetPosition = Target;

		FVector Direction = (Target - Agent.Position).GetSafeNormal();
		float SpeedMultiplier = 1.0f;

		if (Agent.Identity.CrowdStressLevel > 0.5f)
		{
			SpeedMultiplier = FMath::Lerp(1.0f, 0.6f, Agent.Identity.CrowdStressLevel);
		}

		Agent.Position += Direction * Agent.MovementSpeed * SpeedMultiplier * DeltaTime;
	}
}

void UGZMassAI::UpdateTrafficBehavior(float DeltaTime)
{
	for (FGZAgentState& Agent : AgentStates)
	{
		if (Agent.Type != EGZAgentType::Vehicle || !Agent.bIsActive) continue;

		bool bNearAccident = false;
		for (const FGZTrafficAccident& Accident : ActiveAccidents)
		{
			if (FVector::Dist(Agent.Position, Accident.Location) < Accident.CongestionRadius)
			{
				bNearAccident = true;
				FVector AwayFromAccident = (Agent.Position - Accident.Location).GetSafeNormal();
				Agent.TargetPosition += AwayFromAccident * Accident.CongestionRadius;
				break;
			}
		}

		float SpeedMult = 1.0f;
		switch (Agent.DriverType)
		{
		case EGZDriverType::Aggressive:
			SpeedMult = 1.2f;
			break;
		case EGZDriverType::Cautious:
			SpeedMult = 0.8f;
			break;
		case EGZDriverType::Lawful:
			SpeedMult = 1.0f;
			break;
		}

		if (bNearAccident)
		{
			SpeedMult *= 0.3f;
		}

		FVector Direction = (Agent.TargetPosition - Agent.Position).GetSafeNormal();
		Agent.Position += Direction * Agent.MovementSpeed * SpeedMult * DeltaTime;
	}
}

void UGZMassAI::UpdatePoliceBehavior(float DeltaTime, EGZWantedLevel WantedLevel, const FVector& PlayerPosition)
{
	for (int32 i = 0; i < AgentStates.Num(); ++i)
	{
		FGZAgentState& Agent = AgentStates[i];
		if (Agent.Type != EGZAgentType::Police || !Agent.bIsActive) continue;

		Agent.WantedResponse = WantedLevel;

		if (WantedLevel == EGZWantedLevel::None) continue;

		ExecutePoliceTactics(Agent.AgentID, WantedLevel, PlayerPosition);
	}
}

void UGZMassAI::UpdateEmergencyBehavior(float DeltaTime)
{
	for (FGZAgentState& Agent : AgentStates)
	{
		if (Agent.Type != EGZAgentType::Emergency || !Agent.bIsActive) continue;

		FVector Direction = (Agent.TargetPosition - Agent.Position).GetSafeNormal();
		Agent.Position += Direction * Agent.MovementSpeed * 1.5f * DeltaTime;
	}
}

void UGZMassAI::UpdateCrowdDensity(float DeltaTime)
{
	const float CrowdRadius = 500.0f;
	const float HighDensityThreshold = 10.0f;

	for (FGZAgentState& Agent : AgentStates)
	{
		if (Agent.Type != EGZAgentType::Pedestrian) continue;

		int32 NearbyCount = 0;
		for (const FGZAgentState& Other : AgentStates)
		{
			if (Other.AgentID == Agent.AgentID) continue;
			if (Other.Type != EGZAgentType::Pedestrian) continue;
			if (FVector::Dist(Agent.Position, Other.Position) < CrowdRadius)
			{
				NearbyCount++;
			}
		}

		float Density = NearbyCount / HighDensityThreshold;
		Agent.Identity.CrowdStressLevel = FMath::Clamp(Density, 0.0f, 1.0f);
	}
}

void UGZMassAI::UpdateEmotionalStates(float DeltaTime)
{
	for (FGZAgentState& Agent : AgentStates)
	{
		if (Agent.Type != EGZAgentType::Pedestrian) continue;

		EGZEmotionalState NewState = EGZEmotionalState::Calm;

		if (Agent.Identity.CrowdStressLevel > 0.7f)
		{
			NewState = EGZEmotionalState::Anxious;
		}
		else if (Agent.WantedResponse >= EGZWantedLevel::Level3)
		{
			NewState = EGZEmotionalState::Angry;
		}
		else if (Agent.HourOfDay >= 18.0f && Agent.HourOfDay < 22.0f)
		{
			NewState = EGZEmotionalState::Excited;
		}

		Agent.Identity.EmotionalState = NewState;
	}
}

void UGZMassAI::UpdateTrafficAccidents(float DeltaTime)
{
	for (int32 i = ActiveAccidents.Num() - 1; i >= 0; --i)
	{
		ActiveAccidents[i].ElapsedTime += DeltaTime;
		if (ActiveAccidents[i].ElapsedTime >= ActiveAccidents[i].Duration)
		{
			ActiveAccidents.RemoveAt(i);
		}
	}
}

void UGZMassAI::UpdateAgentLOD(float DeltaTime, const FVector& PlayerPosition)
{
	LODUpdateTimer += DeltaTime;
	float NearTick = NearLODTickRate;
	float FarTick = FarLODTickRate;

	if (LODUpdateTimer < FMath::Min(NearTick, FarTick)) return;
	LODUpdateTimer = 0.0f;

	for (FGZAgentState& Agent : AgentStates)
	{
		float Dist = FVector::Dist(Agent.Position, PlayerPosition);
		if (Dist < LODDistanceThreshold)
		{
			Agent.MovementSpeed = Agent.Type == EGZAgentType::Pedestrian ? 1.4f : 13.8f;
		}
		else
		{
			Agent.MovementSpeed = Agent.Type == EGZAgentType::Pedestrian ? 0.5f : 5.0f;
		}
	}
}

void UGZMassAI::ExecutePoliceTactics(int32 AgentID, EGZWantedLevel WantedLevel, const FVector& PlayerPosition)
{
	for (FGZAgentState& Agent : AgentStates)
	{
		if (Agent.AgentID != AgentID || Agent.Type != EGZAgentType::Police) continue;

		switch (WantedLevel)
		{
		case EGZWantedLevel::Level1:
		{
			FVector PatrolDir = FVector(
				FMath::FRandRange(-1.0f, 1.0f),
				FMath::FRandRange(-1.0f, 1.0f),
				0.0f).GetSafeNormal();
			Agent.TargetPosition = Agent.Position + PatrolDir * 2000.0f;
			Agent.MovementSpeed = 2.0f;
			break;
		}
		case EGZWantedLevel::Level2:
		{
			Agent.TargetPosition = PlayerPosition;
			Agent.MovementSpeed = 6.0f;
			break;
		}
		case EGZWantedLevel::Level3:
		{
			Agent.TargetPosition = PlayerPosition + FVector(
				FMath::FRandRange(-500.0f, 500.0f),
				FMath::FRandRange(-500.0f, 500.0f),
				0.0f);
			Agent.MovementSpeed = 8.0f;
			break;
		}
		case EGZWantedLevel::Level4:
		{
			FVector FlankOffset = FVector(
				FMath::FRandRange(-300.0f, 300.0f),
				FMath::FRandRange(-300.0f, 300.0f),
				0.0f);
			Agent.TargetPosition = PlayerPosition + FlankOffset;
			Agent.MovementSpeed = 10.0f;
			break;
		}
		case EGZWantedLevel::Level5:
		{
			FVector PredictPos = PlayerPosition + (PlayerPosition - Agent.Position).GetSafeNormal() * 500.0f;
			Agent.TargetPosition = PredictPos;
			Agent.MovementSpeed = 12.0f;
			break;
		}
		default:
			break;
		}

		FVector Direction = (Agent.TargetPosition - Agent.Position).GetSafeNormal();
		Agent.Position += Direction * Agent.MovementSpeed * 0.016f;
	}
}
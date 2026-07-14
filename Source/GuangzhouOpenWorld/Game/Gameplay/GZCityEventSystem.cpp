#include "GZCityEventSystem.h"
#include "Game/GZGameMode.h"

UGZCityEventSystem::UGZCityEventSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGZCityEventSystem::BeginPlay()
{
	Super::BeginPlay();
	Initialize();
}

void UGZCityEventSystem::Initialize()
{
	if (UWorld* World = GetWorld())
	{
		CachedGameMode = Cast<AGZGameMode>(World->GetAuthGameMode());
		if (CachedGameMode.IsValid() && CachedGameMode->CityEventConfigs.Num() > 0)
		{
			EventConfigs = CachedGameMode->CityEventConfigs;
		}
	}

	if (EventConfigs.Num() == 0)
	{
		const uint8 LastType = static_cast<uint8>(EGZCityEventType::StreetPerformance);
		for (uint8 i = 0; i <= LastType; ++i)
		{
			FCityEventConfig Config;
			Config.EventType = static_cast<EGZCityEventType>(i);
			Config.PreferredDistrict = EGZCityDistrict::TianheCBD;
			Config.SpawnChance = 0.02f;
			Config.MinCooldown = 60.0f;
			Config.MaxRadius = 30000.0f;
			EventConfigs.Add(Config);
		}
	}

	ActiveEvents.Empty();
	CooldownTimers.Empty();
	NextEventId = 1;
}

void UGZCityEventSystem::TickEvents(float DeltaSeconds, float CurrentTime, FVector PlayerLocation, EGZCityDistrict PlayerDistrict)
{
	if (DeltaSeconds <= 0.0f)
	{
		return;
	}

	for (auto& Pair : CooldownTimers)
	{
		Pair.Value = FMath::Max(0.0f, Pair.Value - DeltaSeconds);
	}

	CleanupExpiredEvents(CurrentTime);
	TrySpawnEvent(CurrentTime, PlayerLocation, PlayerDistrict);
}

bool UGZCityEventSystem::TrySpawnEvent(float CurrentTime, FVector PlayerLocation, EGZCityDistrict PlayerDistrict)
{
	if (ActiveEvents.Num() >= MaxConcurrentEvents)
	{
		return false;
	}

	for (const FCityEventConfig& Config : EventConfigs)
	{
		if (IsEventTypeOnCooldown(Config.EventType, CurrentTime))
		{
			continue;
		}

		const float DistrictMultiplier = (Config.PreferredDistrict == PlayerDistrict) ? 1.0f : 0.5f;
		const float SpawnRoll = FMath::FRandRange(0.0f, 1.0f);
		if (SpawnRoll >= Config.SpawnChance * DistrictMultiplier)
		{
			continue;
		}

		FActiveCityEvent NewEvent;
		NewEvent.EventId = NextEventId++;
		NewEvent.EventType = Config.EventType;
		NewEvent.District = PlayerDistrict;
		NewEvent.Location = PickRandomEventLocation(PlayerLocation, Config.MaxRadius);
		NewEvent.SpawnTime = CurrentTime;
		NewEvent.Duration = DefaultEventDuration;
		NewEvent.bDismissed = false;
		NewEvent.bExpired = false;

		ActiveEvents.Add(NewEvent);
		CooldownTimers.Add(static_cast<int32>(Config.EventType), Config.MinCooldown);

		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("CityEventSystem: spawned event %d (type %d) in district %d at %s"),
			NewEvent.EventId, static_cast<int32>(NewEvent.EventType), static_cast<int32>(NewEvent.District), *NewEvent.Location.ToString());

		return true;
	}

	return false;
}

void UGZCityEventSystem::CleanupExpiredEvents(float CurrentTime)
{
	for (int32 i = ActiveEvents.Num() - 1; i >= 0; --i)
	{
		FActiveCityEvent& Event = ActiveEvents[i];
		const float Duration = (Event.Duration > 0.0f) ? Event.Duration : DefaultEventDuration;

		if (Event.bDismissed || (CurrentTime - Event.SpawnTime) >= Duration)
		{
			Event.bExpired = true;
			ActiveEvents.RemoveAtSwap(i);
		}
	}
}

FVector UGZCityEventSystem::PickRandomEventLocation(FVector PlayerLocation, float MaxRadius) const
{
	const float Angle = FMath::FRandRange(0.0f, 2.0f * PI);
	const float Radius = FMath::FRandRange(MaxRadius * 0.2f, MaxRadius);

	const float X = PlayerLocation.X + Radius * FMath::Cos(Angle);
	const float Y = PlayerLocation.Y + Radius * FMath::Sin(Angle);
	const float Z = PlayerLocation.Z;

	return FVector(X, Y, Z);
}

bool UGZCityEventSystem::IsEventTypeOnCooldown(EGZCityEventType EventType, float CurrentTime) const
{
	const float* Cooldown = CooldownTimers.Find(static_cast<int32>(EventType));
	return Cooldown != nullptr && *Cooldown > 0.0f;
}

FCityEventConfig UGZCityEventSystem::GetEventConfig(EGZCityEventType EventType) const
{
	for (const FCityEventConfig& Config : EventConfigs)
	{
		if (Config.EventType == EventType)
		{
			return Config;
		}
	}
	return FCityEventConfig();
}

bool UGZCityEventSystem::DismissEvent(int32 EventId)
{
	for (FActiveCityEvent& Event : ActiveEvents)
	{
		if (Event.EventId == EventId && !Event.bDismissed)
		{
			Event.bDismissed = true;
			if (UWorld* World = GetWorld())
			{
				Event.DismissTime = World->GetTimeSeconds();
			}
			return true;
		}
	}
	return false;
}

bool UGZCityEventSystem::RunLayer1_APICompliance()
{
	return EventConfigs.Num() > 0;
}

bool UGZCityEventSystem::RunLayer2_Syntax()
{
	for (const FCityEventConfig& Config : EventConfigs)
	{
		if (Config.SpawnChance < 0.0f || Config.MinCooldown < 0.0f || Config.MaxRadius <= 0.0f)
		{
			return false;
		}
	}
	return true;
}

bool UGZCityEventSystem::RunLayer3_Parameters()
{
	const int32 ExpectedCount = static_cast<int32>(EGZCityEventType::StreetPerformance) + 1;
	return EventConfigs.Num() == ExpectedCount;
}

bool UGZCityEventSystem::RunLayer4_FunctionalRules()
{
	if (ActiveEvents.Num() > MaxConcurrentEvents)
	{
		return false;
	}

	for (const FActiveCityEvent& Event : ActiveEvents)
	{
		if (!Event.bDismissed && Event.EventId <= 0)
		{
			return false;
		}
	}
	return true;
}

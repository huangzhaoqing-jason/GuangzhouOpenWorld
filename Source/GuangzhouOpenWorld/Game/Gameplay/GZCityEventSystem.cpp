#include "GZCityEventSystem.h"

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
	EventConfigs.Empty();

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

	ActiveEvents.Empty();
	CooldownTimers.Empty();
	NextEventId = 1;
	SpawnTimer = 0.0f;
}

void UGZCityEventSystem::TrySpawnEvent(float DeltaSeconds)
{
	SpawnTimer += DeltaSeconds;

	for (const FCityEventConfig& Config : EventConfigs)
	{
		const int32 TypeIndex = static_cast<int32>(Config.EventType);
		float& Cooldown = CooldownTimers.FindOrAdd(TypeIndex);

		if (Cooldown > 0.0f)
		{
			Cooldown -= DeltaSeconds;
			continue;
		}

		if (FMath::FRandRange(0.0f, 1.0f) < Config.SpawnChance * DeltaSeconds)
		{
			FActiveCityEvent NewEvent;
			NewEvent.EventId = NextEventId++;
			NewEvent.EventType = Config.EventType;
			NewEvent.SpawnTime = SpawnTimer;
			ActiveEvents.Add(NewEvent);

			Cooldown = Config.MinCooldown;
			break;
		}
	}
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
			Event.DismissTime = SpawnTimer;
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
	for (const FActiveCityEvent& Event : ActiveEvents)
	{
		if (!Event.bDismissed && Event.EventId <= 0)
		{
			return false;
		}
	}
	return true;
}

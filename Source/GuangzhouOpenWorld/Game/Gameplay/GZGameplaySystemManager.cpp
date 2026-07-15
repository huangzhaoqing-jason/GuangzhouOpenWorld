#include "GZGameplaySystemManager.h"
#include "GZCityEventSystem.h"
#include "GZDualCharacterSystem.h"
#include "GZSceneInteractionSystem.h"
#include "GZProfessionSystem.h"
#include "GZFloodSystem.h"
#include "GZVehicleModificationSystem.h"
#include "GZNPCLifecycleSystem.h"
#include "GZAnomalySystem.h"
#include "Game/GZPlayerController.h"
#include "Game/GZGameMode.h"

UGZGameplaySystemManager::UGZGameplaySystemManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.0f / 30.0f;
}

void UGZGameplaySystemManager::BeginPlay()
{
	Super::BeginPlay();
	InitializeSystems();
}

void UGZGameplaySystemManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateSystems(DeltaTime);
}

void UGZGameplaySystemManager::InitializeSystems()
{
	if (UWorld* World = GetWorld())
	{
		CachedGameMode = Cast<AGZGameMode>(World->GetAuthGameMode());
	}

	CityEventSystem = NewObject<UGZCityEventSystem>(this);
	DualCharacterSystem = NewObject<UGZDualCharacterSystem>(this);
	SceneInteractionSystem = NewObject<UGZSceneInteractionSystem>(this);
	ProfessionSystem = NewObject<UGZProfessionSystem>(this);
	FloodSystem = NewObject<UGZFloodSystem>(this);
	VehicleModificationSystem = NewObject<UGZVehicleModificationSystem>(this);
	NPCLifecycleSystem = NewObject<UGZNPCLifecycleSystem>(this);
	AnomalySystem = NewObject<UGZAnomalySystem>(this);

	if (CityEventSystem) { CityEventSystem->Initialize(); }
	if (DualCharacterSystem)
	{
		DualCharacterSystem->Initialize();
		DualCharacterSystem->LoadDualCharacterData();
	}
	if (SceneInteractionSystem) { SceneInteractionSystem->Initialize(); }
	if (ProfessionSystem) { ProfessionSystem->Initialize(); }
	if (FloodSystem) { FloodSystem->Initialize(); }
	if (VehicleModificationSystem) { VehicleModificationSystem->Initialize(); }
	if (NPCLifecycleSystem) { NPCLifecycleSystem->Initialize(); }
	if (AnomalySystem) { AnomalySystem->Initialize(); }

	if (DistrictBoundaries.Num() == 0)
	{
		FDistrictBoundary CBD;
		CBD.District = EGZCityDistrict::TianheCBD;
		CBD.MinBounds = FVector(0.0f, 0.0f, -100000.0f);
		CBD.MaxBounds = FVector(100000.0f, 100000.0f, 100000.0f);

		FDistrictBoundary OldTown;
		OldTown.District = EGZCityDistrict::YuexiuOldTown;
		OldTown.MinBounds = FVector(-100000.0f, 0.0f, -100000.0f);
		OldTown.MaxBounds = FVector(0.0f, 100000.0f, 100000.0f);

		FDistrictBoundary Waterfront;
		Waterfront.District = EGZCityDistrict::HaizhuWaterfront;
		Waterfront.MinBounds = FVector(-100000.0f, -100000.0f, -100000.0f);
		Waterfront.MaxBounds = FVector(0.0f, 0.0f, 100000.0f);

		FDistrictBoundary Suburb;
		Suburb.District = EGZCityDistrict::PanyuSuburb;
		Suburb.MinBounds = FVector(0.0f, -100000.0f, -100000.0f);
		Suburb.MaxBounds = FVector(100000.0f, 0.0f, 100000.0f);

		DistrictBoundaries.Add(CBD);
		DistrictBoundaries.Add(OldTown);
		DistrictBoundaries.Add(Waterfront);
		DistrictBoundaries.Add(Suburb);
	}

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("GameplaySystemManager: all gameplay systems initialized."));
}

void UGZGameplaySystemManager::UpdateSystems(float DeltaSeconds)
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	APawn* Pawn = PC ? PC->GetPawn() : nullptr;
	if (!Pawn)
	{
		return;
	}

	const FVector PlayerLocation = Pawn->GetActorLocation();

	DistrictUpdateTimer += DeltaSeconds;
	if (DistrictUpdateTimer >= DistrictUpdateInterval)
	{
		DistrictUpdateTimer = 0.0f;
		CurrentDistrict = DetectDistrictByLocation(PlayerLocation);
		if (CachedGameMode)
		{
			CachedGameMode->SetCurrentDistrict(CurrentDistrict);
		}
	}

	const float CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;

	if (CityEventSystem)
	{
		CityEventSystem->TickEvents(DeltaSeconds, CurrentTime, PlayerLocation, CurrentDistrict);
	}

	if (SceneInteractionSystem)
	{
		SceneInteractionSystem->UpdateActiveShop(PlayerLocation, CurrentDistrict);
	}

	if (FloodSystem && CachedGameMode)
	{
		const float RainIntensity = CachedGameMode->GetDetailedWeatherIntensity();
		FloodSystem->UpdateFlood(RainIntensity, DeltaSeconds);
	}

	if (NPCLifecycleSystem && CachedGameMode)
	{
		NPCLifecycleSystem->OnWeatherChanged(CachedGameMode->GetDetailedWeather());
		NPCLifecycleSystem->UpdateNPCBehavior(DeltaSeconds);
	}

	if (AnomalySystem)
	{
		AnomalySystem->CheckAnomalyTriggers(PlayerLocation);
	}
}

EGZCityDistrict UGZGameplaySystemManager::DetectDistrictByLocation(FVector Location) const
{
	for (const FDistrictBoundary& Boundary : DistrictBoundaries)
	{
		if (Location.X >= Boundary.MinBounds.X && Location.X <= Boundary.MaxBounds.X &&
		    Location.Y >= Boundary.MinBounds.Y && Location.Y <= Boundary.MaxBounds.Y &&
		    Location.Z >= Boundary.MinBounds.Z && Location.Z <= Boundary.MaxBounds.Z)
		{
			return Boundary.District;
		}
	}
	return CurrentDistrict;
}

void UGZGameplaySystemManager::SetDistrictBoundaries(const TArray<FDistrictBoundary>& Boundaries)
{
	DistrictBoundaries = Boundaries;
}

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Game/GZGameMode.h"
#include "GuangzhouOpenWorld.h"
#include "GZGameplaySystemManager.generated.h"

USTRUCT()
struct FDistrictBoundary
{
	GENERATED_BODY()

	UPROPERTY()
	EGZCityDistrict District = EGZCityDistrict::TianheCBD;

	UPROPERTY()
	FVector MinBounds = FVector::ZeroVector;

	UPROPERTY()
	FVector MaxBounds = FVector::ZeroVector;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GUANGZHOUOPENWORLD_API UGZGameplaySystemManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UGZGameplaySystemManager();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void InitializeSystems();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void UpdateSystems(float DeltaSeconds);

	UFUNCTION(BlueprintPure, Category = "Gameplay")
	class UGZCityEventSystem* GetCityEventSystem() const { return CityEventSystem; }

	UFUNCTION(BlueprintPure, Category = "Gameplay")
	class UGZDualCharacterSystem* GetDualCharacterSystem() const { return DualCharacterSystem; }

	UFUNCTION(BlueprintPure, Category = "Gameplay")
	class UGZSceneInteractionSystem* GetSceneInteractionSystem() const { return SceneInteractionSystem; }

	UFUNCTION(BlueprintPure, Category = "Gameplay")
	class UGZProfessionSystem* GetProfessionSystem() const { return ProfessionSystem; }

	UFUNCTION(BlueprintPure, Category = "Gameplay")
	class UGZFloodSystem* GetFloodSystem() const { return FloodSystem; }

	UFUNCTION(BlueprintPure, Category = "Gameplay")
	class UGZVehicleModificationSystem* GetVehicleModificationSystem() const { return VehicleModificationSystem; }

	UFUNCTION(BlueprintPure, Category = "Gameplay")
	class UGZNPCLifecycleSystem* GetNPCLifecycleSystem() const { return NPCLifecycleSystem; }

	UFUNCTION(BlueprintPure, Category = "Gameplay")
	class UGZAnomalySystem* GetAnomalySystem() const { return AnomalySystem; }

	UFUNCTION(BlueprintPure, Category = "Gameplay")
	EGZCityDistrict DetectDistrictByLocation(FVector Location) const;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetDistrictBoundaries(const TArray<FDistrictBoundary>& Boundaries);

protected:
	UPROPERTY()
	class UGZCityEventSystem* CityEventSystem = nullptr;

	UPROPERTY()
	class UGZDualCharacterSystem* DualCharacterSystem = nullptr;

	UPROPERTY()
	class UGZSceneInteractionSystem* SceneInteractionSystem = nullptr;

	UPROPERTY()
	class UGZProfessionSystem* ProfessionSystem = nullptr;

	UPROPERTY()
	class UGZFloodSystem* FloodSystem = nullptr;

	UPROPERTY()
	class UGZVehicleModificationSystem* VehicleModificationSystem = nullptr;

	UPROPERTY()
	class UGZNPCLifecycleSystem* NPCLifecycleSystem = nullptr;

	UPROPERTY()
	class UGZAnomalySystem* AnomalySystem = nullptr;

	UPROPERTY()
	class AGZGameMode* CachedGameMode = nullptr;

	UPROPERTY()
	TArray<FDistrictBoundary> DistrictBoundaries;

	UPROPERTY()
	float DistrictUpdateInterval = 1.0f;

	UPROPERTY()
	float DistrictUpdateTimer = 0.0f;

	UPROPERTY()
	EGZCityDistrict CurrentDistrict = EGZCityDistrict::TianheCBD;
};

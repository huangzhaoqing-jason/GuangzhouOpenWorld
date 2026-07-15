#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Game/Gameplay/GZSurfacePhysics.h"
#include "Game/GZGameMode.h"
#include "GZTrafficConflictManager.generated.h"

USTRUCT()
struct FGZTrafficVehicle
{
	GENERATED_BODY()

	UPROPERTY()
	int32 VehicleId = 0;

	UPROPERTY()
	EGZCityDistrict District = EGZCityDistrict::TianheCBD;

	UPROPERTY()
	FVector Location = FVector::ZeroVector;

	UPROPERTY()
	FVector Forward = FVector::ForwardVector;

	UPROPERTY()
	float Speed = 0.0f;

	UPROPERTY()
	float DesiredSpeed = 600.0f;

	UPROPERTY()
	float FollowingDistance = 400.0f;

	UPROPERTY()
	float BlockedTimer = 0.0f;

	UPROPERTY()
	float HornCooldown = 0.0f;

	UPROPERTY()
	bool bHornTriggered = false;

	UPROPERTY()
	bool bNeedsReroute = false;

	UPROPERTY()
	EGZGuangzhouSurfaceType SurfaceType = EGZGuangzhouSurfaceType::WornConcrete;
};

USTRUCT()
struct FGZTrafficDistrictConfig
{
	GENERATED_BODY()

	UPROPERTY()
	EGZCityDistrict District = EGZCityDistrict::TianheCBD;

	UPROPERTY()
	int32 MaxVehicles = 40;

	UPROPERTY()
	float MinFollowingDistance = 300.0f;

	UPROPERTY()
	float SafeDistance = 600.0f;

	UPROPERTY()
	float RerouteBlockedThreshold = 3.0f;

	UPROPERTY()
	float HornReactionDelay = 0.6f;

	UPROPERTY()
	float NarrowLaneSpeedCap = 400.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTrafficConflictEvent, EGZCityDistrict, District, int32, VehicleId);

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZTrafficConflictManager : public UObject
{
	GENERATED_BODY()

public:
	UGZTrafficConflictManager();

	UFUNCTION(BlueprintCallable)
	void Initialize();

	UFUNCTION(BlueprintCallable)
	int32 RegisterVehicle(EGZCityDistrict District, FVector Location, FVector Forward, float DesiredSpeed, EGZGuangzhouSurfaceType SurfaceType);

	UFUNCTION(BlueprintCallable)
	void UnregisterVehicle(int32 VehicleId);

	UFUNCTION(BlueprintCallable)
	void TickTraffic(float DeltaSeconds, UGZSurfacePhysics* SurfacePhysics, float Wetness01);

	UFUNCTION(BlueprintCallable)
	void HonkAtVehicle(int32 VehicleId);

	UFUNCTION(BlueprintPure)
	int32 GetVehicleCount(EGZCityDistrict District) const;

	UFUNCTION(BlueprintPure)
	const TArray<FGZTrafficVehicle>& GetVehicles() const { return Vehicles; }

	UPROPERTY(BlueprintAssignable)
	FOnTrafficConflictEvent OnVehicleHorned;

	UPROPERTY(BlueprintAssignable)
	FOnTrafficConflictEvent OnVehicleRerouted;

private:
	void RegisterDistrictConfigs();

	const FGZTrafficDistrictConfig* FindDistrictConfig(EGZCityDistrict District) const;

	FGZTrafficVehicle* FindVehicle(int32 VehicleId);

	void UpdateVehicle(FGZTrafficVehicle& Vehicle, float DeltaSeconds, UGZSurfacePhysics* SurfacePhysics, float Wetness01);

	int32 NextVehicleId = 1;

	UPROPERTY()
	TArray<FGZTrafficVehicle> Vehicles;

	UPROPERTY()
	TArray<FGZTrafficDistrictConfig> DistrictConfigs;
};

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Game/GZGameMode.h"
#include "GZMassAI.generated.h"

UENUM(BlueprintType)
enum class EGZOccupation : uint8
{
	Unemployed	UMETA(DisplayName = "Unemployed"),
	OfficeWorker	UMETA(DisplayName = "Office Worker"),
	ShopVendor	UMETA(DisplayName = "Shop Vendor"),
	Driver		UMETA(DisplayName = "Driver"),
	Student		UMETA(DisplayName = "Student"),
	Tourist		UMETA(DisplayName = "Tourist"),
	Police		UMETA(DisplayName = "Police"),
};

UENUM(BlueprintType)
enum class EGZEmotionalState : uint8
{
	Neutral		UMETA(DisplayName = "Neutral"),
	Happy		UMETA(DisplayName = "Happy"),
	Anxious		UMETA(DisplayName = "Anxious"),
	Angry		UMETA(DisplayName = "Angry"),
};

UENUM(BlueprintType)
enum class EGZDriverType : uint8
{
	Aggressive	UMETA(DisplayName = "Aggressive"),
	Cautious	UMETA(DisplayName = "Cautious"),
	Lawful		UMETA(DisplayName = "Lawful"),
};

UENUM(BlueprintType)
enum class EGZAgentLOD : uint8
{
	Near		UMETA(DisplayName = "Near (<100m)"),
	Medium		UMETA(DisplayName = "Medium (100-200m)"),
	Far			UMETA(DisplayName = "Far (>200m)"),
};

UENUM(BlueprintType)
enum class ENPCWeatherResponse : uint8
{
	Normal		UMETA(DisplayName = "Normal"),
	SeekShelter	UMETA(DisplayName = "Seek Shelter"),
	SlowDown	UMETA(DisplayName = "Slow Down"),
	StayIndoor	UMETA(DisplayName = "Stay Indoor"),
};

UENUM(BlueprintType)
enum class EGZAreaType : uint8
{
	Commercial	UMETA(DisplayName = "Commercial"),
	Residential	UMETA(DisplayName = "Residential"),
	Industrial	UMETA(DisplayName = "Industrial"),
	Park		UMETA(DisplayName = "Park"),
	Transport	UMETA(DisplayName = "Transport"),
	Tourist		UMETA(DisplayName = "Tourist"),
};

USTRUCT(BlueprintType)
struct FAgentMemory
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector PlayerLastInteraction = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PlayerLastAction = TEXT("None");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NPCInteractionCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LastInteractionTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsHostile = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ReputationScore = 50.0f;
};

USTRUCT(BlueprintType)
struct FGZAgentIdentity
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AgentID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name = TEXT("Agent");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZOccupation Occupation = EGZOccupation::Unemployed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZEmotionalState EmotionalState = EGZEmotionalState::Neutral;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZDriverType DriverType = EGZDriverType::Lawful;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Age = 25;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector HomeLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector WorkLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ScheduleTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsIndoor = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CurrentLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZAreaType AreaType = EGZAreaType::Commercial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasUmbrella = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsInConversation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ConversationTargetID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAgentMemory Memory;
};

USTRUCT(BlueprintType)
struct FInterNPCDialog
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DialogID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpeakerA_ID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpeakerB_ID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DialogText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsWeatherRelated = false;
};

USTRUCT(BlueprintType)
struct FAgentWeatherAction
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZWeatherType WeatherType = EGZWeatherType::Clear;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasUmbrella = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCoversHead = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPrefersCoveredPaths = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bStaysIndoor = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpeedMultiplier = 1.0f;
};

USTRUCT(BlueprintType)
struct FAgentWeatherState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ENPCWeatherResponse CurrentResponse = ENPCWeatherResponse::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector ShelterLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpeedMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHasReachedShelter = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WeatherResponseTimer = 0.0f;
};

USTRUCT(BlueprintType)
struct FGZNPCSchedule
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HourOfDay = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CommuteProbability = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WorkProbability = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ShopProbability = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RestProbability = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CrowdDensity = 0.6f; // 0.2-1.0 crowd density multiplier for this hour

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpeedMultiplier = 1.0f; // NPC movement speed multiplier for this hour
};

USTRUCT(BlueprintType)
struct FNPCDensityByTime
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HourStart = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HourEnd = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DensityMultiplier = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpeedMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PeriodName;
};

USTRUCT(BlueprintType)
struct FGZPoliceTactics
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WantedLevel = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 OfficerCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseFlanking = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseCover = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUsePredictivePathfinding = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSurround = false;
};

USTRUCT(BlueprintType)
struct FTrafficVehicleState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AgentID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TargetSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BrakingDistance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TurnRadius = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHeadlightsOn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTailLightsOn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInTunnel = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WaterSplashTimer = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WaterSplashIntensity = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DecelerationBuffer = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsCongested = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CongestionLocation = FVector::ZeroVector;
};

UENUM(BlueprintType)
enum class EGZPlayerEquipment : uint8
{
	Unarmed		UMETA(DisplayName = "Unarmed"),
	Melee		UMETA(DisplayName = "Melee"),
	Pistol		UMETA(DisplayName = "Pistol"),
	Rifle		UMETA(DisplayName = "Rifle"),
	HeavyWeapon	UMETA(DisplayName = "Heavy Weapon"),
	Vehicle		UMETA(DisplayName = "Vehicle"),
};

USTRUCT(BlueprintType)
struct FHostileAdaptation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZPlayerEquipment PlayerEquipment = EGZPlayerEquipment::Unarmed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseLongRange = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseCoverMore = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFlankFrequently = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseVehicles = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EngagementDistance = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AccuracyMultiplier = 1.0f;
};

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZMassAI : public UObject
{
	GENERATED_BODY()

public:
	UGZMassAI();

	void Initialize();
	void Tick(float DeltaTime, const FVector& PlayerLocation);
	void RegisterAgent(int32 AgentID, const FGZAgentIdentity& Identity);
	void UnregisterAgent(int32 AgentID);
	void UpdateAgentLOD(const FVector& AgentLocation, const FVector& PlayerLocation, int32 AgentID);
	void UpdatePoliceResponse(int32 WantedLevel, const FVector& TargetLocation);

	void UpdateWeatherResponse(EGZWeatherType Weather, float HourOfDay);
	void ApplyRainShelter();
	void ApplyFogSlowdown();
	void ApplyStormResponse();

	void UpdateNPCInteractions(float DeltaTime, float HourOfDay);
	void UpdateAgentMemory(float DeltaTime, const FVector& PlayerLocation);
	void GenerateInterNPCDialog(int32 AgentA, int32 AgentB);
	void ApplyWeatherActions(EGZWeatherType Weather);
	FAgentWeatherAction GetWeatherActionForAgent(EGZWeatherType Weather, int32 AgentID) const;
	void UpdateCommuteBehavior(float HourOfDay, EGZAreaType Area);
	void SetAreaDensity(EGZAreaType Area, float Density);
	void UpdateRushHour(float HourOfDay);

	UFUNCTION(BlueprintPure)
	int32 GetAgentCount() const { return Agents.Num(); }

	UFUNCTION(BlueprintPure)
	EGZAgentLOD GetAgentLOD(int32 AgentID) const;

	UFUNCTION(BlueprintPure)
	const FGZPoliceTactics& GetPoliceTactics() const { return CurrentTactics; }

	UFUNCTION(BlueprintPure)
	ENPCWeatherResponse GetAgentWeatherResponse(int32 AgentID) const;

	static FGZNPCSchedule GetScheduleForHour(float Hour);
	static FGZPoliceTactics GetPoliceTacticsForWantedLevel(int32 WantedLevel);

	UFUNCTION(BlueprintPure)
	float GetCrowdDensityForHour(float Hour) const;

	UFUNCTION(BlueprintPure)
	float GetNPCSpeedMultiplierForHour(float Hour) const;

private:
	void UpdateAgentSchedules(float DeltaTime, float HourOfDay);
	void UpdateDriverBehavior(float DeltaTime);
	void UpdatePoliceBehavior(float DeltaTime, const FVector& PlayerLocation);
	void AssignAgentLODs(const FVector& PlayerLocation);
	float GetAgentLODTickRate(EGZAgentLOD LOD) const;
	void AssignDriverTypes();
	void HandleAccidentReroute(int32 AgentID, const FVector& AccidentLocation);
	void UpdateAgentWeatherStates(float DeltaTime);
	void ReduceOutdoorDensity(float OutdoorFraction);

	// v5.3 Traffic vehicle physics & lights (subtasks 77-82)
	void UpdateVehiclePhysics(float DeltaTime, EGZWeatherType Weather);
	void UpdateTrafficCongestion(float DeltaTime);
	void UpdateVehicleLights(float DeltaTime, float HourOfDay);
	void TriggerVehicleWaterSplash(int32 AgentID, float Intensity);
	FTrafficVehicleState GetVehicleState(int32 AgentID) const;

	// v5.3 Hostile adaptation to player equipment (subtask 86)
	void UpdateHostileAdaptation(EGZPlayerEquipment Equipment);
	FHostileAdaptation GetHostileAdaptation() const { return CurrentHostileAdaptation; }

	UPROPERTY()
	TMap<int32, FGZAgentIdentity> Agents;

	UPROPERTY()
	TMap<int32, EGZAgentLOD> AgentLODs;

	UPROPERTY()
	TMap<int32, FAgentWeatherState> AgentWeatherStates;

	UPROPERTY()
	FGZPoliceTactics CurrentTactics;

	UPROPERTY()
	TArray<FVector> PolicePositions;

	UPROPERTY()
	TArray<FVector> AccidentLocations;

	UPROPERTY()
	TArray<FInterNPCDialog> ActiveDialogs;

	UPROPERTY()
	TMap<EGZAreaType, float> AreaDensities;

	UPROPERTY()
	TArray<FAgentWeatherAction> WeatherActions;

	UPROPERTY()
	TArray<FNPCDensityByTime> TimeDensityTable;

	UPROPERTY()
	TMap<int32, FTrafficVehicleState> VehicleStates;

	UPROPERTY()
	FHostileAdaptation CurrentHostileAdaptation;

	float RushHourDensityMultiplier = 1.0f;
	float NightSparseDensityMultiplier = 0.2f;
	float DialogUpdateTimer = 0.0f;
	float DialogUpdateInterval = 3.0f;
	float MemoryUpdateTimer = 0.0f;
	float MemoryUpdateInterval = 5.0f;

	int32 NextAgentID = 0;
	int32 MaxAgents = 12000;

	float AgentLODCheckTimer = 0.0f;
	float AgentLODCheckInterval = 0.5f;
	float ScheduleUpdateTimer = 0.0f;
	float ScheduleUpdateInterval = 1.0f;
	float PoliceUpdateTimer = 0.0f;
	float PoliceUpdateInterval = 0.5f;
	float WeatherUpdateTimer = 0.0f;
	float WeatherUpdateInterval = 2.0f;

	EGZWeatherType CurrentWeather = EGZWeatherType::Clear;
	float CurrentHourOfDay = 12.0f;

	static constexpr float NearDistance = 10000.0f;
	static constexpr float MediumDistance = 20000.0f;
	static constexpr float NearTickRate = 60.0f;
	static constexpr float MediumTickRate = 30.0f;
	static constexpr float FarTickRate = 10.0f;
	static constexpr float AccidentRerouteTime = 0.8f;
	static constexpr float AggressiveSpeedMultiplier = 1.2f;
	static constexpr float CautiousSpeedMultiplier = 0.9f;
	static constexpr float LawfulSpeedMultiplier = 1.0f;
	static constexpr float DriverRatioAggressive = 3.0f;
	static constexpr float DriverRatioCautious = 5.0f;
	static constexpr float DriverRatioLawful = 2.0f;
	static constexpr float RainShelterSpeedMultiplier = 1.5f;
	static constexpr float FogSlowdownMultiplier = 0.7f;
	static constexpr float StormOutdoorFraction = 0.05f;
	static constexpr float ShelterSearchRadius = 5000.0f;

	// v5.3 Vehicle physics constants (subtasks 77-82)
	static constexpr float BaseDecelerationBuffer = 0.4f;
	static constexpr float MinTurnRadius = 300.0f;
	static constexpr float TurnRadiusSpeedFactor = 10.0f;
	static constexpr float RainBrakingExtension = 15.0f;
	static constexpr float RainTurnRadiusIncrease = 10.0f;
	static constexpr float TunnelLightThreshold = 0.15f;
	static constexpr float NightLightHourStart = 18.0f;
	static constexpr float NightLightHourEnd = 6.0f;
	static constexpr float CongestionRadius = 800.0f;
	static constexpr float CongestionSpeedThreshold = 200.0f;
	static constexpr int32 CongestionMinVehicles = 4;
};
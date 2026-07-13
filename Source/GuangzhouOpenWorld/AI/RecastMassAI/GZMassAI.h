#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
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

	UFUNCTION(BlueprintPure)
	int32 GetAgentCount() const { return Agents.Num(); }

	UFUNCTION(BlueprintPure)
	EGZAgentLOD GetAgentLOD(int32 AgentID) const;

	UFUNCTION(BlueprintPure)
	const FGZPoliceTactics& GetPoliceTactics() const { return CurrentTactics; }

	static FGZNPCSchedule GetScheduleForHour(float Hour);
	static FGZPoliceTactics GetPoliceTacticsForWantedLevel(int32 WantedLevel);

private:
	void UpdateAgentSchedules(float DeltaTime, float HourOfDay);
	void UpdateDriverBehavior(float DeltaTime);
	void UpdatePoliceBehavior(float DeltaTime, const FVector& PlayerLocation);
	void AssignAgentLODs(const FVector& PlayerLocation);
	float GetAgentLODTickRate(EGZAgentLOD LOD) const;
	void AssignDriverTypes();
	void HandleAccidentReroute(int32 AgentID, const FVector& AccidentLocation);

	UPROPERTY()
	TMap<int32, FGZAgentIdentity> Agents;

	UPROPERTY()
	TMap<int32, EGZAgentLOD> AgentLODs;

	UPROPERTY()
	FGZPoliceTactics CurrentTactics;

	UPROPERTY()
	TArray<FVector> PolicePositions;

	UPROPERTY()
	TArray<FVector> AccidentLocations;

	int32 NextAgentID = 0;
	int32 MaxAgents = 12000;

	float AgentLODCheckTimer = 0.0f;
	float AgentLODCheckInterval = 0.5f;
	float ScheduleUpdateTimer = 0.0f;
	float ScheduleUpdateInterval = 1.0f;
	float PoliceUpdateTimer = 0.0f;
	float PoliceUpdateInterval = 0.5f;

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
};
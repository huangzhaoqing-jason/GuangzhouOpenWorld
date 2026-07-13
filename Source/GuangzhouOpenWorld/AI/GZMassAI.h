#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZMassAI.generated.h"

UENUM(BlueprintType)
enum class EGZAgentType : uint8
{
	Pedestrian	UMETA(DisplayName = "Pedestrian"),
	Vehicle		UMETA(DisplayName = "Vehicle"),
	Police		UMETA(DisplayName = "Police"),
	Emergency	UMETA(DisplayName = "Emergency"),
};

UENUM(BlueprintType)
enum class EGZOccupation : uint8
{
	Office		UMETA(DisplayName = "Office Worker"),
	Retail		UMETA(DisplayName = "Retail Worker"),
	Student		UMETA(DisplayName = "Student"),
	Tourist		UMETA(DisplayName = "Tourist"),
	Elderly		UMETA(DisplayName = "Elderly"),
	Worker		UMETA(DisplayName = "Manual Worker"),
	Unemployed	UMETA(DisplayName = "Unemployed"),
};

UENUM(BlueprintType)
enum class EGZEmotionalState : uint8
{
	Calm	UMETA(DisplayName = "Calm"),
	Anxious	UMETA(DisplayName = "Anxious"),
	Excited	UMETA(DisplayName = "Excited"),
	Angry	UMETA(DisplayName = "Angry"),
};

UENUM(BlueprintType)
enum class EGZDriverType : uint8
{
	Aggressive	UMETA(DisplayName = "Aggressive"),
	Cautious	UMETA(DisplayName = "Cautious"),
	Lawful		UMETA(DisplayName = "Lawful"),
};

UENUM(BlueprintType)
enum class EGZWantedLevel : uint8
{
	None		UMETA(DisplayName = "None"),
	Level1		UMETA(DisplayName = "Level 1 - Patrol"),
	Level2		UMETA(DisplayName = "Level 2 - Pursue"),
	Level3		UMETA(DisplayName = "Level 3 - Roadblocks"),
	Level4		UMETA(DisplayName = "Level 4 - SWAT"),
	Level5		UMETA(DisplayName = "Level 5 - Military"),
};

USTRUCT(BlueprintType)
struct FGZAgentIdentity
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name = TEXT("Citizen");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Age = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZOccupation Occupation = EGZOccupation::Office;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector HomeLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector WorkLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZEmotionalState EmotionalState = EGZEmotionalState::Calm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CrowdStressLevel = 0.0f;
};

USTRUCT(BlueprintType)
struct FGZAgentState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AgentID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZAgentType Type = EGZAgentType::Pedestrian;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TargetPosition = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGZAgentIdentity Identity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeed = 1.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsActive = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HourOfDay = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZWantedLevel WantedResponse = EGZWantedLevel::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZDriverType DriverType = EGZDriverType::Lawful;
};

USTRUCT(BlueprintType)
struct FGZTrafficAccident
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Location = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Severity = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ElapsedTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CongestionRadius = 100.0f;
};

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZMassAI : public UObject
{
	GENERATED_BODY()

public:
	UGZMassAI();

	void Initialize(int32 InMaxAgents);
	void Tick(float DeltaTime, float CurrentHour, EGZWantedLevel WantedLevel, const FVector& PlayerPosition);
	void SpawnAgent(EGZAgentType Type, const FVector& Location, const FGZAgentIdentity& Identity);
	void RemoveAgent(int32 AgentID);
	void ReportTrafficAccident(const FVector& Location, float Severity);
	void SetWantedLevel(EGZWantedLevel Level);

	UFUNCTION(BlueprintPure)
	int32 GetActiveAgentCount() const { return ActiveAgents.Num(); }

	UFUNCTION(BlueprintPure)
	const TArray<FGZAgentState>& GetAgentStates() const { return AgentStates; }

	UFUNCTION(BlueprintPure)
	const TArray<FGZTrafficAccident>& GetActiveAccidents() const { return ActiveAccidents; }

private:
	void UpdatePedestrianBehavior(float DeltaTime, float CurrentHour);
	void UpdateTrafficBehavior(float DeltaTime);
	void UpdatePoliceBehavior(float DeltaTime, EGZWantedLevel WantedLevel, const FVector& PlayerPosition);
	void UpdateEmergencyBehavior(float DeltaTime);
	void UpdateCrowdDensity(float DeltaTime);
	void UpdateEmotionalStates(float DeltaTime);
	void UpdateTrafficAccidents(float DeltaTime);
	void UpdateAgentLOD(float DeltaTime, const FVector& PlayerPosition);
	void ExecutePoliceTactics(int32 AgentID, EGZWantedLevel WantedLevel, const FVector& PlayerPosition);

	UPROPERTY()
	TArray<FGZAgentState> AgentStates;

	UPROPERTY()
	TArray<FGZTrafficAccident> ActiveAccidents;

	int32 MaxAgents = 12000;
	int32 NextAgentID = 0;
	float LODUpdateTimer = 0.0f;
	float EmotionalUpdateTimer = 0.0f;
	float CrowdUpdateTimer = 0.0f;

	static constexpr float NearLODTickRate = 1.0f / 30.0f;
	static constexpr float FarLODTickRate = 1.0f / 5.0f;
	static constexpr float LODDistanceThreshold = 5000.0f;
};
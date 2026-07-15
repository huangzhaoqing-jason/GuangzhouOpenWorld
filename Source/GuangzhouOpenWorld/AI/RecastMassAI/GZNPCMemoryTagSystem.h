#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZNPCMemoryTagSystem.generated.h"

UENUM(BlueprintType)
enum class EGZNPCConflictTag : uint8
{
	VendorDisturbed     UMETA(DisplayName = "Vendor Disturbed"),
	HonkedAt            UMETA(DisplayName = "Honked At"),
	NearCollision       UMETA(DisplayName = "Near Collision"),
	PlayerBlockingPath  UMETA(DisplayName = "Player Blocking Path"),
	PlayerSprintingCrowd UMETA(DisplayName = "Player Sprinting In Crowd"),
	Sympathy            UMETA(DisplayName = "Sympathy"),
};

UENUM(BlueprintType)
enum class EGZNPCConflictStage : uint8
{
	Calm     UMETA(DisplayName = "Calm"),
	Annoyed  UMETA(DisplayName = "Annoyed"),
	Angry    UMETA(DisplayName = "Angry"),
	Hostile  UMETA(DisplayName = "Hostile"),
};

USTRUCT()
struct FGZNPCTagInstance
{
	GENERATED_BODY()

	UPROPERTY()
	EGZNPCConflictTag TagType = EGZNPCConflictTag::VendorDisturbed;

	UPROPERTY()
	float Intensity = 0.0f;

	UPROPERTY()
	float CreatedTime = 0.0f;

	UPROPERTY()
	float LastTriggeredTime = 0.0f;

	UPROPERTY()
	float DecayPerSecond = 5.0f;

	UPROPERTY()
	float SympathyRadius = 0.0f;
};

USTRUCT()
struct FGZNPCEscalationRule
{
	GENERATED_BODY()

	UPROPERTY()
	EGZNPCConflictTag TagType = EGZNPCConflictTag::VendorDisturbed;

	UPROPERTY()
	float AnnoyedThreshold = 30.0f;

	UPROPERTY()
	float AngryThreshold = 70.0f;

	UPROPERTY()
	float HostileThreshold = 100.0f;

	UPROPERTY()
	float DecayPerSecond = 6.0f;

	UPROPERTY()
	float CrowdPropagationIntensity = 8.0f;

	UPROPERTY()
	float CrowdPropagationRadius = 500.0f;
};

USTRUCT()
struct FGZNPCMemory
{
	GENERATED_BODY()

	UPROPERTY()
	int32 NPCId = 0;

	UPROPERTY()
	FVector Location = FVector::ZeroVector;

	UPROPERTY()
	TArray<FGZNPCTagInstance> Tags;

	UPROPERTY()
	EGZNPCConflictStage CurrentStage = EGZNPCConflictStage::Calm;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnNPCStageChanged, int32, NPCId, EGZNPCConflictStage, NewStage, EGZNPCConflictStage, OldStage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCrowdReactionTriggered, int32, SourceNPCId, int32, AffectedNPCId);

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZNPCMemoryTagSystem : public UObject
{
	GENERATED_BODY()

public:
	UGZNPCMemoryTagSystem();

	UFUNCTION(BlueprintCallable)
	void Initialize();

	UFUNCTION(BlueprintCallable)
	void RegisterNPC(int32 NPCId, FVector Location);

	UFUNCTION(BlueprintCallable)
	void UnregisterNPC(int32 NPCId);

	UFUNCTION(BlueprintCallable)
	void SetNPCLocation(int32 NPCId, FVector Location);

	UFUNCTION(BlueprintCallable)
	void AddTag(int32 NPCId, EGZNPCConflictTag TagType, float Intensity, float CurrentTime);

	UFUNCTION(BlueprintCallable)
	void TickMemory(float DeltaSeconds, float CurrentTime);

	UFUNCTION(BlueprintPure)
	EGZNPCConflictStage GetNPCStage(int32 NPCId) const;

	UFUNCTION(BlueprintPure)
	float GetTagIntensity(int32 NPCId, EGZNPCConflictTag TagType) const;

	UFUNCTION(BlueprintPure)
	const TArray<FGZNPCMemory>& GetAllMemories() const { return NPCMemories; }

	UPROPERTY(BlueprintAssignable)
	FOnNPCStageChanged OnNPCStageChanged;

	UPROPERTY(BlueprintAssignable)
	FOnCrowdReactionTriggered OnCrowdReactionTriggered;

private:
	void RegisterEscalationRules();

	const FGZNPCEscalationRule* FindRule(EGZNPCConflictTag TagType) const;

	FGZNPCMemory* FindMemory(int32 NPCId);

	const FGZNPCMemory* FindMemoryRead(int32 NPCId) const;

	EGZNPCConflictStage ComputeStage(const FGZNPCMemory& Memory) const;

	void DecayTags(FGZNPCMemory& Memory, float DeltaSeconds);

	void PropagateCrowdReaction(float CurrentTime);

	UPROPERTY()
	TArray<FGZNPCMemory> NPCMemories;

	UPROPERTY()
	TArray<FGZNPCEscalationRule> EscalationRules;
};

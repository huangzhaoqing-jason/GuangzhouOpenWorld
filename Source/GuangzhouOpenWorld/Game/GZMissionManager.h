#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZMissionManager.generated.h"

UENUM(BlueprintType)
enum class EGZMissionType : uint8
{
	MainStory	UMETA(DisplayName = "Main Story"),
	Side		UMETA(DisplayName = "Side Mission"),
	Daily		UMETA(DisplayName = "Daily"),
	Racing		UMETA(DisplayName = "Racing"),
	Collection	UMETA(DisplayName = "Collection"),
	Wanted		UMETA(DisplayName = "Wanted"),
};

UENUM(BlueprintType)
enum class EGZMissionStatus : uint8
{
	Locked		UMETA(DisplayName = "Locked"),
	Available	UMETA(DisplayName = "Available"),
	Active		UMETA(DisplayName = "Active"),
	Completed	UMETA(DisplayName = "Completed"),
	Failed		UMETA(DisplayName = "Failed"),
};

USTRUCT(BlueprintType)
struct FGZMissionObjective
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TargetCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsCompleted = false;
};

USTRUCT(BlueprintType)
struct FGZMissionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MissionID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MissionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZMissionType MissionType = EGZMissionType::Side;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZMissionStatus Status = EGZMissionStatus::Available;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGZMissionObjective> Objectives;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector StartLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RewardMoney = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RewardXP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RewardItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsTimeLimited = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeLimit = 0.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMissionChanged, const FName&, MissionID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMissionCompleted, const FName&, MissionID, bool, bSuccess);

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZMissionManager : public UObject
{
	GENERATED_BODY()

public:
	UGZMissionManager();

	UFUNCTION(BlueprintCallable, Category = "Mission")
	void StartMission(const FName& MissionID);

	UFUNCTION(BlueprintCallable, Category = "Mission")
	void CompleteMission(const FName& MissionID);

	UFUNCTION(BlueprintCallable, Category = "Mission")
	void FailMission(const FName& MissionID);

	UFUNCTION(BlueprintCallable, Category = "Mission")
	void UpdateObjectiveProgress(const FName& MissionID, int32 ObjectiveIndex, int32 Progress);

	UFUNCTION(BlueprintPure, Category = "Mission")
	FGZMissionData* GetMission(const FName& MissionID);

	UFUNCTION(BlueprintPure, Category = "Mission")
	TArray<FGZMissionData> GetActiveMissions() const;

	UFUNCTION(BlueprintPure, Category = "Mission")
	TArray<FGZMissionData> GetAvailableMissions() const;

	UFUNCTION(BlueprintPure, Category = "Mission")
	float GetMissionProgress(const FName& MissionID) const;

	UFUNCTION(BlueprintCallable, Category = "Mission")
	void RegisterMission(const FGZMissionData& Mission);

	/** Load Act1 seeds from Content/Data or Docs/Data missions_act1.json. */
	UFUNCTION(BlueprintCallable, Category = "Mission")
	int32 LoadAct1MissionsFromJson();

	UPROPERTY(BlueprintAssignable, Category = "Mission")
	FOnMissionChanged OnMissionStarted;

	UPROPERTY(BlueprintAssignable, Category = "Mission")
	FOnMissionChanged OnMissionUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Mission")
	FOnMissionCompleted OnMissionCompleted;

private:
	TArray<FGZMissionData> MissionRegistry;
};
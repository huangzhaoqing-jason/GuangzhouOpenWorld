#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Game/GZGameMode.h"
#include "GZP1GameMode.generated.h"

UENUM(BlueprintType)
enum class EP1MissionState : uint8
{
	Intro     UMETA(DisplayName = "Intro"),
	Drive     UMETA(DisplayName = "Drive"),
	Explore   UMETA(DisplayName = "Explore"),
	Conflict  UMETA(DisplayName = "Conflict"),
	Interact  UMETA(DisplayName = "Interact"),
	Complete  UMETA(DisplayName = "Complete"),
};

UCLASS()
class GUANGZHOUOPENWORLD_API AGZP1GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGZP1GameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure)
	class UGZSurfacePhysics* GetSurfacePhysics() const { return SurfacePhysics; }

	UFUNCTION(BlueprintPure)
	class UGZTrafficConflictManager* GetTrafficManager() const { return TrafficManager; }

	UFUNCTION(BlueprintPure)
	class UGZNPCMemoryTagSystem* GetNPCMemory() const { return NPCMemory; }

	UFUNCTION(BlueprintPure)
	class UGZVerticalPathSystem* GetVerticalPath() const { return VerticalPath; }

	UFUNCTION(BlueprintPure)
	class UGZPhysicsInteractionSystem* GetPhysicsInteraction() const { return PhysicsInteraction; }

	UFUNCTION(BlueprintPure)
	class UGZLiquidGlassPresenter* GetPresenter() const { return Presenter; }

	UFUNCTION(BlueprintPure)
	EP1MissionState GetMissionState() const { return MissionState; }

protected:
	void InitializeP1Systems();
	void ApplyPerformanceFallback();
	void BuildP1SliceData();
	void TickMission(float DeltaSeconds);
	void UpdatePresenter(float DeltaSeconds);

	FVector GetPlayerPawnLocation() const;

	UPROPERTY()
	class UGZSurfacePhysics* SurfacePhysics = nullptr;

	UPROPERTY()
	class UGZTrafficConflictManager* TrafficManager = nullptr;

	UPROPERTY()
	class UGZNPCMemoryTagSystem* NPCMemory = nullptr;

	UPROPERTY()
	class UGZVerticalPathSystem* VerticalPath = nullptr;

	UPROPERTY()
	class UGZPhysicsInteractionSystem* PhysicsInteraction = nullptr;

	UPROPERTY()
	class UGZLiquidGlassPresenter* Presenter = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "P1 Slice")
	FVector MissionDriveTarget = FVector(20000.0f, 0.0f, 0.0f);

	UPROPERTY(EditDefaultsOnly, Category = "P1 Slice")
	FVector MissionArcadeCenter = FVector(12000.0f, 8000.0f, 0.0f);

	UPROPERTY(EditDefaultsOnly, Category = "P1 Slice")
	FVector MissionShopLocation = FVector(5000.0f, 5000.0f, 0.0f);

	UPROPERTY(EditDefaultsOnly, Category = "P1 Slice")
	float DriveAcceptRadius = 1500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "P1 Slice")
	float ArcadeAcceptRadius = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "P1 Slice")
	float Wetness01 = 0.35f;

	UPROPERTY(EditDefaultsOnly, Category = "P1 Performance")
	bool bApplyPerformanceFallback = true;

private:
	EP1MissionState MissionState = EP1MissionState::Intro;
	float MissionTimer = 0.0f;
	float CurrentTime = 0.0f;
	int32 MissionNPCId = INDEX_NONE;
	int32 MissionObjectId = INDEX_NONE;
	bool bMissionObjectThrown = false;
};

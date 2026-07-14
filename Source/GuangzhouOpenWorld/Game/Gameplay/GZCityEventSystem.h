#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Game/GZGameMode.h"
#include "GuangzhouOpenWorld.h"
#include "GZCityEventSystem.generated.h"

USTRUCT()
struct FActiveCityEvent
{
	GENERATED_BODY()

	UPROPERTY() int32 EventId = 0;
	UPROPERTY() EGZCityEventType EventType = EGZCityEventType::CommercialDispute;
	UPROPERTY() float SpawnTime = 0.0f;
	UPROPERTY() float DismissTime = 0.0f;
	UPROPERTY() bool bDismissed = false;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GUANGZHOUOPENWORLD_API UGZCityEventSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	UGZCityEventSystem();

	UFUNCTION(BlueprintCallable) void Initialize();
	UFUNCTION(BlueprintCallable) void TrySpawnEvent(float DeltaSeconds);
	UFUNCTION(BlueprintPure) FCityEventConfig GetEventConfig(EGZCityEventType EventType) const;
	UFUNCTION(BlueprintCallable) bool DismissEvent(int32 EventId);

	UFUNCTION(BlueprintCallable) bool RunLayer1_APICompliance();
	UFUNCTION(BlueprintCallable) bool RunLayer2_Syntax();
	UFUNCTION(BlueprintCallable) bool RunLayer3_Parameters();
	UFUNCTION(BlueprintCallable) bool RunLayer4_FunctionalRules();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FCityEventConfig> EventConfigs;
	UPROPERTY() TArray<FActiveCityEvent> ActiveEvents;
	UPROPERTY() TMap<int32, float> CooldownTimers;
	UPROPERTY() int32 NextEventId = 1;
	UPROPERTY() float SpawnTimer = 0.0f;
};

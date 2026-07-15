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
	UPROPERTY() EGZCityDistrict District = EGZCityDistrict::TianheCBD;
	UPROPERTY() FVector Location = FVector::ZeroVector;
	UPROPERTY() float SpawnTime = 0.0f;
	UPROPERTY() float DismissTime = 0.0f;
	UPROPERTY() float Duration = 60.0f;
	UPROPERTY() bool bDismissed = false;
	UPROPERTY() bool bExpired = false;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GUANGZHOUOPENWORLD_API UGZCityEventSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	UGZCityEventSystem();

	UFUNCTION(BlueprintCallable) void Initialize();
	UFUNCTION(BlueprintCallable) void TickEvents(float DeltaSeconds, float CurrentTime, FVector PlayerLocation, EGZCityDistrict PlayerDistrict);
	UFUNCTION(BlueprintCallable) bool TrySpawnEvent(float CurrentTime, FVector PlayerLocation, EGZCityDistrict PlayerDistrict);
	UFUNCTION(BlueprintPure) FCityEventConfig GetEventConfig(EGZCityEventType EventType) const;
	UFUNCTION(BlueprintCallable) bool DismissEvent(int32 EventId);
	UFUNCTION(BlueprintCallable) void CleanupExpiredEvents(float CurrentTime);
	UFUNCTION(BlueprintPure) int32 GetActiveEventCount() const { return ActiveEvents.Num(); }
	UFUNCTION(BlueprintPure) const TArray<FActiveCityEvent>& GetActiveEvents() const { return ActiveEvents; }

	UFUNCTION(BlueprintCallable)
	void LoadActiveEvents(const TArray<FActiveCityEvent>& InActiveEvents);

	UFUNCTION(BlueprintCallable) bool RunLayer1_APICompliance();
	UFUNCTION(BlueprintCallable) bool RunLayer2_Syntax();
	UFUNCTION(BlueprintCallable) bool RunLayer3_Parameters();
	UFUNCTION(BlueprintCallable) bool RunLayer4_FunctionalRules();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	FVector PickRandomEventLocation(FVector PlayerLocation, float MaxRadius) const;

	UFUNCTION()
	bool IsEventTypeOnCooldown(EGZCityEventType EventType, float CurrentTime) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FCityEventConfig> EventConfigs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 MaxConcurrentEvents = 8;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float DefaultEventDuration = 120.0f;

	UPROPERTY() TArray<FActiveCityEvent> ActiveEvents;
	UPROPERTY() TMap<int32, float> CooldownTimers;
	UPROPERTY() int32 NextEventId = 1;
	UPROPERTY() TWeakObjectPtr<class AGZGameMode> CachedGameMode;
};

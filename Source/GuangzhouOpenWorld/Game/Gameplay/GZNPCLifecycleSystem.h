#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Game/GZGameMode.h"
#include "GuangzhouOpenWorld.h"
#include "GZNPCLifecycleSystem.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GUANGZHOUOPENWORLD_API UGZNPCLifecycleSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	UGZNPCLifecycleSystem();

	UFUNCTION(BlueprintCallable)
	void Initialize();

	UFUNCTION(BlueprintCallable)
	void UpdateNPCBehavior(float DeltaSeconds);

	UFUNCTION(BlueprintCallable)
	FNPCBehaviorProfile GetBehaviorProfileForDistrict(EGZCityDistrict District);

	UFUNCTION(BlueprintCallable)
	void OnWeatherChanged(EGZWeatherStateDetailed NewWeather);

	UFUNCTION(BlueprintCallable)
	bool RunLayer1_APICompliance();

	UFUNCTION(BlueprintCallable)
	bool RunLayer2_Syntax();

	UFUNCTION(BlueprintCallable)
	bool RunLayer3_Parameters();

	UFUNCTION(BlueprintCallable)
	bool RunLayer4_FunctionalRules();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FNPCBehaviorProfile> BehaviorProfiles;

	UPROPERTY(BlueprintReadOnly)
	EGZNPCBehaviorState CurrentBehaviorState = EGZNPCBehaviorState::Walking;

	UPROPERTY(BlueprintReadOnly)
	EGZWeatherStateDetailed CurrentWeather = EGZWeatherStateDetailed::Overcast;
};

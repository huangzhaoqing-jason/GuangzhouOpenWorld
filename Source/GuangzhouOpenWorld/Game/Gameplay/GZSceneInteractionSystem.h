#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Game/GZGameMode.h"
#include "GuangzhouOpenWorld.h"
#include "GZSceneInteractionSystem.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GUANGZHOUOPENWORLD_API UGZSceneInteractionSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	UGZSceneInteractionSystem();

	UFUNCTION(BlueprintCallable)
	void Initialize();

	UFUNCTION(BlueprintCallable)
	bool CheckShopState(float Hour, EGZWeatherStateDetailed Weather);

	UFUNCTION(BlueprintCallable)
	bool TryEnterIndoor();

	UFUNCTION(BlueprintCallable)
	bool TryExitIndoor();

	UFUNCTION(BlueprintCallable)
	void UpdateActiveShop(FVector PlayerLocation, EGZCityDistrict PlayerDistrict);

	UFUNCTION(BlueprintPure)
	int32 GetActiveShopIndex() const { return ActiveShopIndex; }

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
	TArray<FShopInteractionConfig> ShopConfigs;

	UPROPERTY(BlueprintReadOnly)
	bool bIsIndoor = false;

	UPROPERTY(BlueprintReadOnly)
	int32 ActiveShopIndex = INDEX_NONE;
};

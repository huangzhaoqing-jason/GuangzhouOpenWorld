#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Game/GZGameMode.h"
#include "GuangzhouOpenWorld.h"
#include "GZAnomalySystem.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GUANGZHOUOPENWORLD_API UGZAnomalySystem : public UActorComponent
{
	GENERATED_BODY()

public:
	UGZAnomalySystem();

	UFUNCTION(BlueprintCallable)
	void Initialize();

	UFUNCTION(BlueprintCallable)
	void CheckAnomalyTriggers(FVector PlayerLocation);

	UFUNCTION(BlueprintCallable)
	bool ActivateAnomaly(int32 Index);

	UFUNCTION(BlueprintCallable)
	bool DeactivateAnomaly(int32 Index);

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
	TArray<FAnomalyPointConfig> AnomalyConfigs;

	UPROPERTY(BlueprintReadOnly)
	TArray<uint8> ActiveAnomalies;

	UPROPERTY(BlueprintReadOnly)
	FVector LastPlayerLocation = FVector::ZeroVector;
};

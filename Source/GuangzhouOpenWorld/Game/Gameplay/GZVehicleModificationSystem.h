#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Game/GZGameMode.h"
#include "GuangzhouOpenWorld.h"
#include "GZVehicleModificationSystem.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GUANGZHOUOPENWORLD_API UGZVehicleModificationSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	UGZVehicleModificationSystem();

	UFUNCTION(BlueprintCallable) void Initialize();
	UFUNCTION(BlueprintCallable) void ApplyModification(EGZVehicleModPart Part);
	UFUNCTION(BlueprintPure) FVehicleModConfig GetCurrentModConfig(EGZVehicleModPart Part) const;
	UFUNCTION(BlueprintCallable) void ResetAllMods();

	UFUNCTION(BlueprintCallable) bool RunLayer1_APICompliance();
	UFUNCTION(BlueprintCallable) bool RunLayer2_Syntax();
	UFUNCTION(BlueprintCallable) bool RunLayer3_Parameters();
	UFUNCTION(BlueprintCallable) bool RunLayer4_FunctionalRules();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<EGZVehicleModPart, FVehicleModConfig> ModConfigs;
	UPROPERTY() TMap<EGZVehicleModPart, float> AppliedLevels;
};

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Game/GZGameMode.h"
#include "GuangzhouOpenWorld.h"
#include "GZFloodSystem.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GUANGZHOUOPENWORLD_API UGZFloodSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	UGZFloodSystem();

	UFUNCTION(BlueprintCallable)
	void Initialize();

	UFUNCTION(BlueprintCallable)
	void UpdateFlood(float RainIntensity, float DeltaSeconds);

	UFUNCTION(BlueprintPure)
	float GetFloodHeightAtLocation(FVector Location) const;

	UFUNCTION(BlueprintPure)
	bool IsStreetBlocked(FVector Location) const;

	// MCP 4-layer self-check functions
	UFUNCTION(BlueprintCallable)
	bool RunLayer1_APICompliance();

	UFUNCTION(BlueprintCallable)
	bool RunLayer2_Syntax();

	UFUNCTION(BlueprintCallable)
	bool RunLayer3_Parameters();

	UFUNCTION(BlueprintCallable)
	bool RunLayer4_FunctionalRules();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	FFloodParams FloodParams;

	UPROPERTY()
	float CurrentFloodHeight = 0.0f;

	UPROPERTY()
	TWeakObjectPtr<class AGZGameMode> CachedGameMode;
};

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Game/GZGameMode.h"
#include "GuangzhouOpenWorld.h"
#include "GZProfessionSystem.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GUANGZHOUOPENWORLD_API UGZProfessionSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	UGZProfessionSystem();

	UFUNCTION(BlueprintCallable)
	void Initialize();

	UFUNCTION(BlueprintCallable)
	void SetPlayerProfession(EGZCharacterRole Role);

	UFUNCTION(BlueprintPure)
	FProfessionConfig GetProfessionConfig(EGZCharacterRole Role) const;

	UFUNCTION(BlueprintPure)
	float GetNPCReactionModifier() const;

	UFUNCTION(BlueprintPure)
	EGZCharacterRole GetCurrentRole() const { return CurrentRole; }

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
	EGZCharacterRole CurrentRole = EGZCharacterRole::UrbanExplorer;

	UPROPERTY()
	TWeakObjectPtr<class AGZGameMode> CachedGameMode;
};

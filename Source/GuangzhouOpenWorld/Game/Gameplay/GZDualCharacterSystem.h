#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Game/GZGameMode.h"
#include "GuangzhouOpenWorld.h"
#include "GZDualCharacterSystem.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GUANGZHOUOPENWORLD_API UGZDualCharacterSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	UGZDualCharacterSystem();

	UFUNCTION(BlueprintCallable)
	void Initialize();

	UFUNCTION(BlueprintCallable)
	void SwitchToCharacter(int32 Index);

	UFUNCTION(BlueprintPure)
	int32 GetActiveCharacterIndex() const;

	UFUNCTION(BlueprintCallable)
	void SaveDualCharacterData();

	UFUNCTION(BlueprintCallable)
	void LoadDualCharacterData();

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
	TArray<FDualCharacterData> CharacterData;

	UPROPERTY(BlueprintReadOnly)
	int32 ActiveCharacterIndex = 0;
};

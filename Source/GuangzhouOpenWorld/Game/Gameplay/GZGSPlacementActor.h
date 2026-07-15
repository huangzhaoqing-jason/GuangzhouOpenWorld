#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Scene/GSRendering/GZGSAsset.h"
#include "GZGSPlacementActor.generated.h"

UCLASS()
class GUANGZHOUOPENWORLD_API AGZGSPlacementActor : public AActor
{
	GENERATED_BODY()

public:
	AGZGSPlacementActor();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gaussian Splatting")
	UGZGSAsset* GSAsset = nullptr;

protected:
	void RebuildComponentsFromAsset();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* RootScene = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* CollisionBox = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* TriggerBox = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UNavModifierComponent* NavModifier = nullptr;
};

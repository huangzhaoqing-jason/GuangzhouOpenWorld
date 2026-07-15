#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GZGSAsset.generated.h"

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZGSAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gaussian Splatting")
	FString SourceFilePath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gaussian Splatting")
	FBox LocalBounds = FBox(FVector(-500.0f), FVector(500.0f));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	FVector CollisionExtent = FVector(500.0f, 500.0f, 500.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	FVector TriggerExtent = FVector(550.0f, 550.0f, 550.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
	bool bAffectsNavigation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
	TSubclassOf<class UNavAreaBase> NavAreaClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
	float NavModifierWeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
	int32 SplatCount = 0;

	UFUNCTION(BlueprintPure)
	FBox GetWorldBounds(const FTransform& WorldTransform) const;
};

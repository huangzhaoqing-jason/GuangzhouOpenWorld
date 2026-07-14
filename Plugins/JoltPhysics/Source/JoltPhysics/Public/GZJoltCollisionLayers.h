#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZJoltCollisionLayers.generated.h"

UENUM(BlueprintType)
enum class EJoltCollisionLayer : uint8
{
	Ground      UMETA(DisplayName="Ground"),
	Building    UMETA(DisplayName="Building"),
	Vehicle     UMETA(DisplayName="Vehicle"),
	NPC         UMETA(DisplayName="NPC"),
	Prop        UMETA(DisplayName="Prop"),
	Vegetation  UMETA(DisplayName="Vegetation"),
	Count       UMETA(Hidden),
};

USTRUCT(BlueprintType)
struct FZJoltCollisionMatrix
{
	GENERATED_BODY()

	UPROPERTY() bool bGroundCollides[6];
	UPROPERTY() bool bBuildingCollides[6];
	UPROPERTY() bool bVehicleCollides[6];
	UPROPERTY() bool bNPCCollides[6];
	UPROPERTY() bool bPropCollides[6];
	UPROPERTY() bool bVegetationCollides[6];

	void InitializeDefault();
	bool ShouldCollide(EJoltCollisionLayer A, EJoltCollisionLayer B) const;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class JOLTPHYSICS_API UGZJoltCollisionLayers : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="JoltCollision")
	static UGZJoltCollisionLayers* GetDefaultLayers();

	UFUNCTION(BlueprintCallable, Category="JoltCollision")
	bool ShouldLayersCollide(EJoltCollisionLayer A, EJoltCollisionLayer B) const;

	UFUNCTION(BlueprintCallable, Category="JoltCollision")
	bool ShouldUseDetailedCollision(EJoltCollisionLayer Layer, float DistanceMeters) const;

private:
	FZJoltCollisionMatrix Matrix;
};

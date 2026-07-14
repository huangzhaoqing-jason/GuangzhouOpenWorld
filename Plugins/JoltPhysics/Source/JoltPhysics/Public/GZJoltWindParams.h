#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZJoltWindParams.generated.h"

UENUM(BlueprintType)
enum class EJoltWindObjectType : uint8
{
	MetalSign       UMETA(DisplayName="Metal Sign"),
	ClothBanner     UMETA(DisplayName="Cloth Banner"),
	ThickBranch     UMETA(DisplayName="Thick Branch"),
	ThinBranch      UMETA(DisplayName="Thin Branch"),
	Leaf            UMETA(DisplayName="Leaf"),
	Conifer         UMETA(DisplayName="Conifer"),
	TallTreeCrown   UMETA(DisplayName="Tall Tree Crown"),
	LowShrub        UMETA(DisplayName="Low Shrub"),
};

USTRUCT(BlueprintType)
struct FZJoltWindData
{
	GENERATED_BODY()

	UPROPERTY() FVector AverageWind = FVector(100.0f, 0.0f, 0.0f);
	UPROPERTY() float GustStrength = 0.0f;
	UPROPERTY() FVector GustDirection = FVector::ForwardVector;
};

USTRUCT(BlueprintType)
struct FZJoltWindObjectParams
{
	GENERATED_BODY()

	UPROPERTY() EJoltWindObjectType ObjectType = EJoltWindObjectType::ThickBranch;
	UPROPERTY() float MassKg = 10.0f;
	UPROPERTY() float Stiffness = 4.0f;
	UPROPERTY() float MinPeriod = 2.0f;
	UPROPERTY() float MaxPeriod = 12.0f;
	UPROPERTY() float MaxSwingAngle = 10.0f;
	UPROPERTY() float SwingScale = 1.0f;
	UPROPERTY() float LocalVariation = 1.0f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class JOLTPHYSICS_API UGZJoltWindParams : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="JoltWind")
	static FZJoltWindObjectParams GetParams(EJoltWindObjectType Type);

	UFUNCTION(BlueprintCallable, Category="JoltWind")
	static float ComputeGustIntervalSeconds();

	UFUNCTION(BlueprintCallable, Category="JoltWind")
	static float ClampSwingAngle(EJoltWindObjectType Type, float Angle);
};

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZDestructionSystem.generated.h"

UENUM(BlueprintType)
enum class EGZDestructibleMaterial : uint8
{
	Glass	UMETA(DisplayName = "Glass"),
	Wall	UMETA(DisplayName = "Wall"),
	Metal	UMETA(DisplayName = "Metal"),
	Wood	UMETA(DisplayName = "Wood"),
};

USTRUCT(BlueprintType)
struct FGZMaterialDestructionProps
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZDestructibleMaterial Material = EGZDestructibleMaterial::Wall;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FractureToughness = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FragmentSizeMin = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FragmentSizeMax = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FragmentCountMin = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FragmentCountMax = 16;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ScatterVelocity = 500.0f;
};

USTRUCT(BlueprintType)
struct FGZDebrisFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector AngularVelocity = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Scale = FVector::OneVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Lifetime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxLifetime = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZDestructibleMaterial Material = EGZDestructibleMaterial::Wall;
};

USTRUCT(BlueprintType)
struct FGZImpactData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector ImpactPoint = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector ImpactVelocity = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ImpactMass = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector ImpactNormal = FVector::UpVector;
};

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZDestructionSystem : public UObject
{
	GENERATED_BODY()

public:
	UGZDestructionSystem();

	void Initialize(int32 InMaxDebrisLimit);
	void ApplyDestruction(const FGZImpactData& Impact, EGZDestructibleMaterial Material, const FVector& ObjectBoundsMin, const FVector& ObjectBoundsMax);
	void Tick(float DeltaTime);
	void SetMaxDebrisLimit(int32 Limit);

	UFUNCTION(BlueprintPure)
	int32 GetActiveDebrisCount() const { return ActiveDebris.Num(); }

	UFUNCTION(BlueprintPure)
	int32 GetMaxDebrisLimit() const { return MaxDebrisLimit; }

	UFUNCTION(BlueprintPure)
	const TArray<FGZDebrisFragment>& GetActiveDebris() const { return ActiveDebris; }

	static FGZMaterialDestructionProps GetMaterialProps(EGZDestructibleMaterial Material);

private:
	void GenerateVoronoiFragments(const FGZImpactData& Impact, const FGZMaterialDestructionProps& Props,
		const FVector& BoundsMin, const FVector& BoundsMax, TArray<FGZDebrisFragment>& OutFragments);
	void SimulateDebrisPhysics(float DeltaTime);
	void RecycleExpiredDebris();
	void CullExcessDebris();

	UPROPERTY()
	TArray<FGZDebrisFragment> ActiveDebris;

	int32 MaxDebrisLimit = 5000;
	static constexpr float Gravity = 9.81f;
	static constexpr float DebrisMaxLifetime = 120.0f;
};
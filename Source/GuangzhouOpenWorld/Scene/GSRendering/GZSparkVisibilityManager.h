#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZSparkVisibilityManager.generated.h"

USTRUCT(BlueprintType)
struct FSparkViewFrustum
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FVector Position = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
	FVector Direction = FVector::ForwardVector;

	UPROPERTY(BlueprintReadWrite)
	float HorizontalFOV = 90.0f;

	UPROPERTY(BlueprintReadWrite)
	float VerticalFOV = 60.0f;

	UPROPERTY(BlueprintReadWrite)
	float FarPlane = 10000.0f;
};

/**
 * Spark 2.0 可见性管理器：仅渲染玩家视锥体内高精度泼溅，
 * 视锥外资源自动卸载或降级为低精度代理。
 */
UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZSparkVisibilityManager : public UObject
{
	GENERATED_BODY()

public:
	UGZSparkVisibilityManager();

	UFUNCTION(BlueprintCallable, Category = "Spark Visibility")
	void Initialize();

	/** 更新相机视锥体，触发可见性重新计算 */
	UFUNCTION(BlueprintCallable, Category = "Spark Visibility")
	void UpdateViewFrustum(const FSparkViewFrustum& Frustum);

	/** 注册一个 3DGS 资产位置与范围 */
	UFUNCTION(BlueprintCallable, Category = "Spark Visibility")
	void RegisterAsset(int32 AssetId, const FVector& Location, float Radius);

	/** 注销资产 */
	UFUNCTION(BlueprintCallable, Category = "Spark Visibility")
	void UnregisterAsset(int32 AssetId);

	/** 查询资产当前可见性级别：0=不可见/卸载, 1=低精度, 2=高精度 */
	UFUNCTION(BlueprintPure, Category = "Spark Visibility")
	int32 GetVisibilityLevel(int32 AssetId) const;

	/** 当前视锥内高精度资产数量 */
	UFUNCTION(BlueprintPure, Category = "Spark Visibility")
	int32 GetHighPriorityCount() const { return HighPriorityAssetIds.Num(); }

private:
	void RecomputeVisibility();
	bool IsInsideFrustum(const FVector& Location, float Radius) const;

	UPROPERTY()
	FSparkViewFrustum CurrentFrustum;

	UPROPERTY()
	TMap<int32, FVector> AssetLocations;

	UPROPERTY()
	TMap<int32, float> AssetRadii;

	UPROPERTY()
	TMap<int32, int32> AssetVisibilityLevels;

	UPROPERTY()
	TArray<int32> HighPriorityAssetIds;

	UPROPERTY()
	bool bInitialized = false;
};

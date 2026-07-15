#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Camera/CameraComponent.h"
#include "GZSparkRendererBridge.generated.h"

USTRUCT(BlueprintType)
struct FSparkQualitySettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxSplatsPerFrame = 1000000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RenderScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseDepthSorting = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OpacityMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float VisibilityDistance = 20000.0f;
};

/**
 * Spark 2.0 渲染管线桥接。
 * UE5.8 作为逻辑容器，不直接渲染 3DGS；此桥接负责将相机/环境/质量参数同步给 Spark。
 * 真实 Spark 渲染在外部 SwiftUI / Metal 上下文中执行，通过共享纹理与 UE5 后处理保持一致。
 */
UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZSparkRendererBridge : public UObject
{
	GENERATED_BODY()

public:
	UGZSparkRendererBridge();

	UFUNCTION(BlueprintCallable)
	void Initialize();

	/** 绑定玩家相机，提取视锥体与后处理参数供 Spark 使用 */
	UFUNCTION(BlueprintCallable)
	void BindPlayerCamera(UCameraComponent* Camera);

	/** 同步后处理参数，确保 SwiftUI Liquid Glass 与 Spark 画面色调一致 */
	UFUNCTION(BlueprintCallable)
	void SyncPostProcessWithUI(UCameraComponent* Camera);

	/** 设置环境变量，驱动 Spark 材质 Shader */
	UFUNCTION(BlueprintCallable)
	void SetEnvironmentParameters(float InHumidity01, float InWindSpeed, float InLightIntensity, float InRainIntensity);

	UFUNCTION(BlueprintCallable)
	void SetQualityLevel(int32 InLevel);

	UFUNCTION(BlueprintPure)
	bool IsSparkConnected() const { return bSparkConnected; }

	UFUNCTION(BlueprintPure)
	const FSparkQualitySettings& GetQualitySettings() const { return QualitySettings; }

	UFUNCTION(BlueprintPure)
	UCameraComponent* GetBoundCamera() const { return BoundCamera; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSparkQualitySettings QualitySettings;

private:
	void UpdateSparkParameters();

	UPROPERTY()
	UCameraComponent* BoundCamera = nullptr;

	float Humidity01 = 0.0f;
	float WindSpeed = 0.0f;
	float LightIntensity = 1.0f;
	float RainIntensity = 0.0f;

	bool bSparkConnected = false;
};

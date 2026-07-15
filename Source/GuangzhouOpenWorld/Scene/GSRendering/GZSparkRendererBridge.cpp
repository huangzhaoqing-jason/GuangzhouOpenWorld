#include "Scene/GSRendering/GZSparkRendererBridge.h"
#include "Engine/Engine.h"

UGZSparkRendererBridge::UGZSparkRendererBridge()
{
}

void UGZSparkRendererBridge::Initialize()
{
	bSparkConnected = false;
	BoundCamera = nullptr;
}

void UGZSparkRendererBridge::BindPlayerCamera(UCameraComponent* Camera)
{
	if (!Camera)
	{
		return;
	}

	BoundCamera = Camera;
	bSparkConnected = true;

	SyncPostProcessWithUI(Camera);
	UpdateSparkParameters();
}

void UGZSparkRendererBridge::SyncPostProcessWithUI(UCameraComponent* Camera)
{
	if (!Camera)
	{
		return;
	}

	// 统一 Liquid Glass UI 与 Spark 场景的后处理参数
	Camera->PostProcessSettings.bOverride_AutoExposureMethod = true;
	Camera->PostProcessSettings.AutoExposureMethod = AEM_Histogram;
	Camera->PostProcessSettings.bOverride_AutoExposureBias = true;
	Camera->PostProcessSettings.AutoExposureBias = 0.0f;
	Camera->PostProcessSettings.bOverride_WhiteTemp = true;
	Camera->PostProcessSettings.WhiteTemp = 6500.0f;
	Camera->PostProcessSettings.bOverride_ColorGamma = true;
	Camera->PostProcessSettings.ColorGamma = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
	Camera->PostProcessSettings.bOverride_BloomIntensity = true;
	Camera->PostProcessSettings.BloomIntensity = 0.6f;
	Camera->PostProcessSettings.bOverride_VignetteIntensity = true;
	Camera->PostProcessSettings.VignetteIntensity = 0.25f;
}

void UGZSparkRendererBridge::SetEnvironmentParameters(float InHumidity01, float InWindSpeed, float InLightIntensity, float InRainIntensity)
{
	Humidity01 = FMath::Clamp(InHumidity01, 0.0f, 1.0f);
	WindSpeed = FMath::Max(InWindSpeed, 0.0f);
	LightIntensity = FMath::Max(InLightIntensity, 0.0f);
	RainIntensity = FMath::Clamp(InRainIntensity, 0.0f, 1.0f);

	UpdateSparkParameters();
}

void UGZSparkRendererBridge::SetQualityLevel(int32 InLevel)
{
	QualitySettings.RenderScale = FMath::Clamp(1.0f - (InLevel * 0.1f), 0.5f, 1.0f);
	QualitySettings.MaxSplatsPerFrame = FMath::Clamp(2000000 - (InLevel * 300000), 500000, 2000000);
	QualitySettings.bUseDepthSorting = InLevel < 3;
	QualitySettings.OpacityMultiplier = FMath::Clamp(1.0f - (InLevel * 0.05f), 0.7f, 1.0f);
	QualitySettings.VisibilityDistance = FMath::Clamp(30000.0f - (InLevel * 4000.0f), 10000.0f, 30000.0f);
}

void UGZSparkRendererBridge::UpdateSparkParameters()
{
	// 此函数将参数写入共享内存 / IPC，供外部 Spark 2.0 渲染器读取
	// Linux Headless 下仅保持状态，不参与实际渲染
	UE_LOG(LogTemp, Log, TEXT("[SparkBridge] Humidity=%.2f Wind=%.2f Light=%.2f Rain=%.2f"),
		Humidity01, WindSpeed, LightIntensity, RainIntensity);
}

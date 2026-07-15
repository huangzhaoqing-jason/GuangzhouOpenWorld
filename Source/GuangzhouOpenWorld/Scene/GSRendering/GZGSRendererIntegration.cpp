#include "Scene/GSRendering/GZGSRendererIntegration.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Engine.h"
#include "Modules/ModuleManager.h"

UGZGSRendererIntegration::UGZGSRendererIntegration()
{
}

void UGZGSRendererIntegration::Initialize()
{
	bNativePluginLoaded = false;
	BlendMID = nullptr;
	BoundCamera = nullptr;

#if !PLATFORM_LINUX
	// 创建 GS 渲染目标，供后处理材质采样
	GSTarget = NewObject<UTextureRenderTarget2D>(this);
	GSTarget->InitAutoFormat(1920, 1080);
	GSTarget->ClearColor = FLinearColor::Transparent;
	GSTarget->bAutoGenerateMips = false;
	GSTarget->UpdateResource();
#endif
}

bool UGZGSRendererIntegration::LoadNativePlugin()
{
#if !PLATFORM_LINUX
	// 优先尝试加载 UE5 原生 Gaussian Splatting 插件模块
	const TArray<FName> CandidateModules =
	{
		FName("GaussianSplatting"),
		FName("PostFXGSRenderer"),
		FName("GSRenderer")
	};

	for (const FName ModuleName : CandidateModules)
	{
		if (FModuleManager::Get().IsModuleLoaded(ModuleName))
		{
			bNativePluginLoaded = true;
			return true;
		}

		if (FModuleManager::Get().ModuleExists(ModuleName.ToString()))
		{
			FModuleManager::Get().LoadModule(ModuleName);
			bNativePluginLoaded = true;
			return true;
		}
	}
#endif

	return bNativePluginLoaded;
}

void UGZGSRendererIntegration::SetupPostProcessBridge(UCameraComponent* Camera)
{
	if (!Camera)
	{
		return;
	}

	BoundCamera = Camera;

#if !PLATFORM_LINUX
	if (BlendMaterial)
	{
		BlendMID = UMaterialInstanceDynamic::Create(BlendMaterial, this);
		if (BlendMID)
		{
			UpdateMaterialParameters();
			Camera->PostProcessSettings.WeightedBlendables.Array.Empty();
			Camera->PostProcessSettings.WeightedBlendables.Array.Add(FWeightedBlendable(1.0f, BlendMID));
		}
	}
#endif
}

void UGZGSRendererIntegration::ApplySharedPostProcess(UCameraComponent* Camera)
{
	if (!Camera)
	{
		return;
	}

	// 统一 Liquid Glass UI 与 GS 场景的后处理参数
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

void UGZGSRendererIntegration::SetEnvironmentParameters(float InHumidity01, float InWindSpeed, float InLightIntensity, float InRainIntensity)
{
	Humidity01 = FMath::Clamp(InHumidity01, 0.0f, 1.0f);
	WindSpeed = FMath::Max(InWindSpeed, 0.0f);
	LightIntensity = FMath::Max(InLightIntensity, 0.0f);
	RainIntensity = FMath::Clamp(InRainIntensity, 0.0f, 1.0f);

	UpdateMaterialParameters();
}

void UGZGSRendererIntegration::SetQualityLevel(int32 InLevel)
{
	QualitySettings.RenderScale = FMath::Clamp(1.0f - (InLevel * 0.1f), 0.5f, 1.0f);
	QualitySettings.MaxSplatsPerFrame = FMath::Clamp(2000000 - (InLevel * 300000), 500000, 2000000);
	QualitySettings.bUseDepthSorting = InLevel < 3;
	QualitySettings.OpacityMultiplier = FMath::Clamp(1.0f - (InLevel * 0.05f), 0.7f, 1.0f);
}

void UGZGSRendererIntegration::UpdateMaterialParameters()
{
#if !PLATFORM_LINUX
	if (!BlendMID)
	{
		return;
	}

	BlendMID->SetScalarParameterValue(FName("GS_Humidity"), Humidity01);
	BlendMID->SetScalarParameterValue(FName("GS_WindSpeed"), WindSpeed);
	BlendMID->SetScalarParameterValue(FName("GS_LightIntensity"), LightIntensity);
	BlendMID->SetScalarParameterValue(FName("GS_RainIntensity"), RainIntensity);
	BlendMID->SetScalarParameterValue(FName("GS_Opacity"), QualitySettings.OpacityMultiplier);
	BlendMID->SetScalarParameterValue(FName("GS_RenderScale"), QualitySettings.RenderScale);

	if (GSTarget)
	{
		BlendMID->SetTextureParameterValue(FName("GS_SceneTexture"), GSTarget);
	}
#endif
}

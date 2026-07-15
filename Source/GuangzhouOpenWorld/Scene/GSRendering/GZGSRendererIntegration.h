#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialInterface.h"
#include "Camera/CameraComponent.h"
#include "GZGSRendererIntegration.generated.h"

USTRUCT(BlueprintType)
struct FGSQualitySettings
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
};

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZGSRendererIntegration : public UObject
{
	GENERATED_BODY()

public:
	UGZGSRendererIntegration();

	UFUNCTION(BlueprintCallable)
	void Initialize();

	UFUNCTION(BlueprintCallable)
	bool LoadNativePlugin();

	UFUNCTION(BlueprintCallable)
	void SetupPostProcessBridge(UCameraComponent* Camera);

	UFUNCTION(BlueprintCallable)
	void ApplySharedPostProcess(UCameraComponent* Camera);

	UFUNCTION(BlueprintCallable)
	void SetEnvironmentParameters(float InHumidity01, float InWindSpeed, float InLightIntensity, float InRainIntensity);

	UFUNCTION(BlueprintCallable)
	void SetQualityLevel(int32 InLevel);

	UFUNCTION(BlueprintPure)
	bool IsNativePluginLoaded() const { return bNativePluginLoaded; }

	UFUNCTION(BlueprintPure)
	UTextureRenderTarget2D* GetGSTarget() const { return GSTarget; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* BlendMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGSQualitySettings QualitySettings;

private:
	void UpdateMaterialParameters();

	UPROPERTY()
	UTextureRenderTarget2D* GSTarget = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* BlendMID = nullptr;

	UPROPERTY()
	UCameraComponent* BoundCamera = nullptr;

	float Humidity01 = 0.0f;
	float WindSpeed = 0.0f;
	float LightIntensity = 1.0f;
	float RainIntensity = 0.0f;

	bool bNativePluginLoaded = false;
};

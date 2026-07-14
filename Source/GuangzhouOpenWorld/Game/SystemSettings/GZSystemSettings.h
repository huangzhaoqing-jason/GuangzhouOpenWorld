#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SystemSettings/GZSystemSettings.generated.h"

UENUM(BlueprintType)
enum class EGZQualityPreset : uint8
{
	UltraLow	UMETA(DisplayName = "Ultra Low"),
	Low		UMETA(DisplayName = "Low"),
	Medium	UMETA(DisplayName = "Medium"),
	High		UMETA(DisplayName = "High"),
	Ultra		UMETA(DisplayName = "Ultra"),
};

UENUM(BlueprintType)
enum class EGZFrameRateLimit : uint8
{
	Limit30		UMETA(DisplayName = "30 FPS"),
	Limit60		UMETA(DisplayName = "60 FPS"),
	Unlimited	UMETA(DisplayName = "Unlimited"),
};

UENUM(BlueprintType)
enum class EGZSettingsCategory : uint8
{
	Graphics		UMETA(DisplayName = "Graphics"),
	Performance	UMETA(DisplayName = "Performance"),
	Audio			UMETA(DisplayName = "Audio"),
	Controls		UMETA(DisplayName = "Controls"),
	Display			UMETA(DisplayName = "Display"),
	Account			UMETA(DisplayName = "Account & Privacy"),
	Saves			UMETA(DisplayName = "Saves"),
};

UENUM(BlueprintType)
enum class EGZDisplayMode : uint8
{
	Fullscreen			UMETA(DisplayName = "Fullscreen"),
	Windowed			UMETA(DisplayName = "Windowed"),
	BorderlessWindowed	UMETA(DisplayName = "Borderless Windowed"),
};

USTRUCT(BlueprintType)
struct FGZGraphicsSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZQualityPreset QualityPreset = EGZQualityPreset::Ultra;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LightPrecision = 1024;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 GlassRefractionSamples = 16;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableTSR = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TSRQuality = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ResolutionScale = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableRainEffects = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableVegetationWind = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableCellBlend = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableMultiLayerGlass = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Brightness = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Contrast = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ColorTemperature = 6500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FieldOfView = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableRayTracing = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableRTGI = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableRTShadows = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableRTReflections = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableRTAO = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableDLSS = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableFSR = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableFrameGen = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DLSSQuality = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FSRQuality = 3;
};

USTRUCT(BlueprintType)
struct FGZDisplaySettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZDisplayMode DisplayMode = EGZDisplayMode::Fullscreen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint Resolution = FIntPoint(1920, 1080);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bVSync = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShowFPS = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Language = TEXT("zh-CN");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSubtitleEnabled = true;
};

USTRUCT(BlueprintType)
struct FGZPerformanceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGZFrameRateLimit FrameRateLimit = EGZFrameRateLimit::Limit60;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BackgroundFrameRateLimit = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PreloadChunkCount = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NaniteQuality = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LumenBounces = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableTemporalSuperSampling = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AdaptiveResolutionTransitionSpeed = 0.2f;
};

USTRUCT(BlueprintType)
struct FGZAudioSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MasterVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MusicVolume = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SFXVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WeatherSFXVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float VehicleVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnable3DSpatialAudio = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ReverbIntensity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableDolbyAtmos = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnable3DAudioHeadphones = true;
};

USTRUCT(BlueprintType)
struct FGZControlSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MouseSensitivity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ControllerSensitivity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bInvertYAxis = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ViewSmoothing = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnableMotionBlur = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MotionBlurIntensity = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FKey MoveForwardKey = EKeys::W;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FKey MoveBackwardKey = EKeys::S;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FKey MoveLeftKey = EKeys::A;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FKey MoveRightKey = EKeys::D;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FKey JumpKey = EKeys::SpaceBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FKey InteractKey = EKeys::E;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FKey SprintKey = EKeys::LeftShift;
};

USTRUCT(BlueprintType)
struct FGZAccountPrivacySettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHidePlayerID = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShareLocationOnline = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAutoCloudSync = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAutoLogin = false;
};

USTRUCT(BlueprintType)
struct FGZSaveSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAutoSaveEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AutoSaveIntervalMinutes = 15.0f;
};

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZSystemSettings : public UObject
{
	GENERATED_BODY()

public:
	UGZSystemSettings();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ApplyPreset(EGZQualityPreset Preset);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SaveAllSettings();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void LoadAllSettings();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ApplyAllSettingsToEngine();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ResetAllToDefaults();

	UFUNCTION(BlueprintPure, Category = "Settings")
	const FGZGraphicsSettings& GetGraphicsSettings() const { return Graphics; }

	UFUNCTION(BlueprintPure, Category = "Settings")
	const FGZPerformanceSettings& GetPerformanceSettings() const { return Performance; }

	UFUNCTION(BlueprintPure, Category = "Settings")
	const FGZAudioSettings& GetAudioSettings() const { return Audio; }

	UFUNCTION(BlueprintPure, Category = "Settings")
	const FGZControlSettings& GetControlSettings() const { return Controls; }

	UFUNCTION(BlueprintPure, Category = "Settings")
	const FGZDisplaySettings& GetDisplaySettings() const { return Display; }

	UFUNCTION(BlueprintPure, Category = "Settings")
	const FGZAccountPrivacySettings& GetAccountPrivacySettings() const { return AccountPrivacy; }

	UFUNCTION(BlueprintPure, Category = "Settings")
	const FGZSaveSettings& GetSaveSettings() const { return Saves; }

	// Graphics setters
	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetQualityPreset(EGZQualityPreset Preset);

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetResolutionScale(float Scale);

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetBrightness(float Brightness);

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetContrast(float Contrast);

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetColorTemperature(float Temp);

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetFieldOfView(float FOV);

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void ToggleRainEffects(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void ToggleVegetationWind(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void ToggleRayTracing(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void ToggleRTGI(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void ToggleRTShadows(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void ToggleRTReflections(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void ToggleRTAO(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void ToggleDLSS(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void ToggleFSR(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void ToggleFrameGen(bool bEnabled);

	// Performance setters
	UFUNCTION(BlueprintCallable, Category = "Settings|Performance")
	void SetFrameRateLimit(EGZFrameRateLimit Limit);

	UFUNCTION(BlueprintCallable, Category = "Settings|Performance")
	void SetPreloadChunkCount(int32 Count);

	// Audio setters
	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void SetMasterVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void SetMusicVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void SetSFXVolume(float Volume);

	// Control setters
	UFUNCTION(BlueprintCallable, Category = "Settings|Controls")
	void SetMouseSensitivity(float Sensitivity);

	UFUNCTION(BlueprintCallable, Category = "Settings|Controls")
	void SetInvertY(bool bInvert);

	UFUNCTION(BlueprintCallable, Category = "Settings|Controls")
	void SetViewSmoothing(float Smoothness);

	// Account setters
	UFUNCTION(BlueprintCallable, Category = "Settings|Account")
	void SetAutoCloudSync(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Settings|Account")
	void SetAutoLogin(bool bEnabled);

	// Save setters
	UFUNCTION(BlueprintCallable, Category = "Settings|Save")
	void SetAutoSaveEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Settings|Save")
	void SetAutoSaveInterval(float Minutes);

	// Display setters
	UFUNCTION(BlueprintCallable, Category = "Settings|Display")
	void SetDisplayMode(EGZDisplayMode Mode);

	UFUNCTION(BlueprintCallable, Category = "Settings|Display")
	void SetResolution(const FIntPoint& Resolution);

	UFUNCTION(BlueprintCallable, Category = "Settings|Display")
	void SetVSync(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Settings|Display")
	void SetShowFPS(bool bShow);

	UFUNCTION(BlueprintCallable, Category = "Settings|Display")
	void SetLanguage(const FString& Language);

	UFUNCTION(BlueprintCallable, Category = "Settings|Display")
	void SetSubtitleEnabled(bool bEnabled);

	// Apply individual settings to engine console variables
	void ApplyGraphicsSettings();
	void ApplyPerformanceSettings();
	void ApplyAudioSettings();
	void ApplyControlSettings();
	void ApplyDisplaySettings();

private:
	FGZGraphicsSettings Graphics;
	FGZPerformanceSettings Performance;
	FGZAudioSettings Audio;
	FGZControlSettings Controls;
	FGZDisplaySettings Display;
	FGZAccountPrivacySettings AccountPrivacy;
	FGZSaveSettings Saves;

	FString SettingsFilePath;
};
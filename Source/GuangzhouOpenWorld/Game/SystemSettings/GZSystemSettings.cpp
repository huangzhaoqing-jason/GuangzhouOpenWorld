#include "Game/SystemSettings/GZSystemSettings.h"
#include "GuangzhouOpenWorld.h"
#include "Internationalization/Internationalization.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonReader.h"
#include "Dom/JsonObject.h"
#include "GameFramework/GameUserSettings.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"

UGZSystemSettings::UGZSystemSettings()
{
	SettingsFilePath = FPaths::ProjectSavedDir() / TEXT("SystemSettings.json");
}

// ===== Presets =====
void UGZSystemSettings::SetQualityPreset(EGZQualityPreset Preset)
{
	Graphics.QualityPreset = Preset;
	ApplyPreset(Preset);
}

void UGZSystemSettings::ApplyPreset(EGZQualityPreset Preset)
{
	switch (Preset)
	{
	case EGZQualityPreset::UltraLow:
		Graphics.ResolutionScale = 70.0f;
		Graphics.bEnableTSR = false;
		Graphics.LightPrecision = 256;
		Graphics.GlassRefractionSamples = 4;
		Graphics.bEnableRainEffects = false;
		Graphics.bEnableVegetationWind = false;
		Graphics.bEnableCellBlend = false;
		Graphics.bEnableMultiLayerGlass = false;
		Performance.FrameRateLimit = EGZFrameRateLimit::Limit30;
		Performance.PreloadChunkCount = 2;
		Performance.NaniteQuality = 0;
		break;

	case EGZQualityPreset::Low:
		Graphics.ResolutionScale = 80.0f;
		Graphics.bEnableTSR = true;
		Graphics.LightPrecision = 512;
		Graphics.GlassRefractionSamples = 6;
		Graphics.bEnableRainEffects = false;
		Graphics.bEnableVegetationWind = false;
		Graphics.bEnableCellBlend = false;
		Graphics.bEnableMultiLayerGlass = false;
		Performance.FrameRateLimit = EGZFrameRateLimit::Limit30;
		Performance.PreloadChunkCount = 4;
		Performance.NaniteQuality = 2;
		break;

	case EGZQualityPreset::Medium:
		Graphics.ResolutionScale = 90.0f;
		Graphics.bEnableTSR = true;
		Graphics.LightPrecision = 768;
		Graphics.GlassRefractionSamples = 8;
		Graphics.bEnableRainEffects = true;
		Graphics.bEnableVegetationWind = true;
		Graphics.bEnableCellBlend = true;
		Graphics.bEnableMultiLayerGlass = false;
		Performance.FrameRateLimit = EGZFrameRateLimit::Limit60;
		Performance.PreloadChunkCount = 6;
		Performance.NaniteQuality = 3;
		break;

	case EGZQualityPreset::High:
		Graphics.ResolutionScale = 100.0f;
		Graphics.bEnableTSR = true;
		Graphics.LightPrecision = 1024;
		Graphics.GlassRefractionSamples = 12;
		Graphics.bEnableRainEffects = true;
		Graphics.bEnableVegetationWind = true;
		Graphics.bEnableCellBlend = true;
		Graphics.bEnableMultiLayerGlass = true;
		Performance.FrameRateLimit = EGZFrameRateLimit::Limit60;
		Performance.PreloadChunkCount = 8;
		Performance.NaniteQuality = 4;
		break;

	case EGZQualityPreset::Ultra:
		Graphics.ResolutionScale = 100.0f;
		Graphics.bEnableTSR = true;
		Graphics.LightPrecision = 2048;
		Graphics.GlassRefractionSamples = 16;
		Graphics.bEnableRainEffects = true;
		Graphics.bEnableVegetationWind = true;
		Graphics.bEnableCellBlend = true;
		Graphics.bEnableMultiLayerGlass = true;
		Performance.FrameRateLimit = EGZFrameRateLimit::Unlimited;
		Performance.PreloadChunkCount = 12;
		Performance.NaniteQuality = 4;
		break;
	}

	ApplyAllSettingsToEngine();
	SaveAllSettings();
}

// ===== Persistence =====
void UGZSystemSettings::SaveAllSettings()
{
	TSharedPtr<FJsonObject> RootObj = MakeShareable(new FJsonObject);

	TSharedPtr<FJsonObject> GraphicsObj = MakeShareable(new FJsonObject);
	GraphicsObj->SetNumberField(TEXT("QualityPreset"), (int32)Graphics.QualityPreset);
	GraphicsObj->SetNumberField(TEXT("LightPrecision"), Graphics.LightPrecision);
	GraphicsObj->SetNumberField(TEXT("GlassRefractionSamples"), Graphics.GlassRefractionSamples);
	GraphicsObj->SetBoolField(TEXT("bEnableTSR"), Graphics.bEnableTSR);
	GraphicsObj->SetNumberField(TEXT("TSRQuality"), Graphics.TSRQuality);
	GraphicsObj->SetNumberField(TEXT("ResolutionScale"), Graphics.ResolutionScale);
	GraphicsObj->SetBoolField(TEXT("bEnableRainEffects"), Graphics.bEnableRainEffects);
	GraphicsObj->SetBoolField(TEXT("bEnableVegetationWind"), Graphics.bEnableVegetationWind);
	GraphicsObj->SetBoolField(TEXT("bEnableCellBlend"), Graphics.bEnableCellBlend);
	GraphicsObj->SetBoolField(TEXT("bEnableMultiLayerGlass"), Graphics.bEnableMultiLayerGlass);
	GraphicsObj->SetNumberField(TEXT("Brightness"), Graphics.Brightness);
	GraphicsObj->SetNumberField(TEXT("Contrast"), Graphics.Contrast);
	GraphicsObj->SetNumberField(TEXT("ColorTemperature"), Graphics.ColorTemperature);
	GraphicsObj->SetNumberField(TEXT("FieldOfView"), Graphics.FieldOfView);
	GraphicsObj->SetBoolField(TEXT("bEnableRayTracing"), Graphics.bEnableRayTracing);
	GraphicsObj->SetBoolField(TEXT("bEnableRTGI"), Graphics.bEnableRTGI);
	GraphicsObj->SetBoolField(TEXT("bEnableRTShadows"), Graphics.bEnableRTShadows);
	GraphicsObj->SetBoolField(TEXT("bEnableRTReflections"), Graphics.bEnableRTReflections);
	GraphicsObj->SetBoolField(TEXT("bEnableRTAO"), Graphics.bEnableRTAO);
	GraphicsObj->SetBoolField(TEXT("bEnableDLSS"), Graphics.bEnableDLSS);
	GraphicsObj->SetBoolField(TEXT("bEnableFSR"), Graphics.bEnableFSR);
	GraphicsObj->SetBoolField(TEXT("bEnableFrameGen"), Graphics.bEnableFrameGen);
	GraphicsObj->SetNumberField(TEXT("DLSSQuality"), Graphics.DLSSQuality);
	GraphicsObj->SetNumberField(TEXT("FSRQuality"), Graphics.FSRQuality);
	RootObj->SetObjectField(TEXT("Graphics"), GraphicsObj);

	TSharedPtr<FJsonObject> PerfObj = MakeShareable(new FJsonObject);
	PerfObj->SetNumberField(TEXT("FrameRateLimit"), (int32)Performance.FrameRateLimit);
	PerfObj->SetNumberField(TEXT("BackgroundFrameRateLimit"), Performance.BackgroundFrameRateLimit);
	PerfObj->SetNumberField(TEXT("PreloadChunkCount"), Performance.PreloadChunkCount);
	PerfObj->SetNumberField(TEXT("NaniteQuality"), Performance.NaniteQuality);
	PerfObj->SetNumberField(TEXT("LumenBounces"), Performance.LumenBounces);
	PerfObj->SetBoolField(TEXT("bEnableTemporalSuperSampling"), Performance.bEnableTemporalSuperSampling);
	PerfObj->SetNumberField(TEXT("AdaptiveResolutionTransitionSpeed"), Performance.AdaptiveResolutionTransitionSpeed);
	RootObj->SetObjectField(TEXT("Performance"), PerfObj);

	TSharedPtr<FJsonObject> AudioObj = MakeShareable(new FJsonObject);
	AudioObj->SetNumberField(TEXT("MasterVolume"), Audio.MasterVolume);
	AudioObj->SetNumberField(TEXT("MusicVolume"), Audio.MusicVolume);
	AudioObj->SetNumberField(TEXT("SFXVolume"), Audio.SFXVolume);
	AudioObj->SetNumberField(TEXT("WeatherSFXVolume"), Audio.WeatherSFXVolume);
	AudioObj->SetNumberField(TEXT("VehicleVolume"), Audio.VehicleVolume);
	AudioObj->SetBoolField(TEXT("bEnable3DSpatialAudio"), Audio.bEnable3DSpatialAudio);
	RootObj->SetObjectField(TEXT("Audio"), AudioObj);

	TSharedPtr<FJsonObject> CtrlObj = MakeShareable(new FJsonObject);
	CtrlObj->SetNumberField(TEXT("MouseSensitivity"), Controls.MouseSensitivity);
	CtrlObj->SetNumberField(TEXT("ControllerSensitivity"), Controls.ControllerSensitivity);
	CtrlObj->SetBoolField(TEXT("bInvertYAxis"), Controls.bInvertYAxis);
	CtrlObj->SetNumberField(TEXT("ViewSmoothing"), Controls.ViewSmoothing);
	CtrlObj->SetBoolField(TEXT("bEnableMotionBlur"), Controls.bEnableMotionBlur);
	RootObj->SetObjectField(TEXT("Controls"), CtrlObj);

	TSharedPtr<FJsonObject> DisplayObj = MakeShareable(new FJsonObject);
	DisplayObj->SetNumberField(TEXT("DisplayMode"), (int32)Display.DisplayMode);
	DisplayObj->SetNumberField(TEXT("ResolutionX"), Display.Resolution.X);
	DisplayObj->SetNumberField(TEXT("ResolutionY"), Display.Resolution.Y);
	DisplayObj->SetBoolField(TEXT("bVSync"), Display.bVSync);
	DisplayObj->SetBoolField(TEXT("bShowFPS"), Display.bShowFPS);
	DisplayObj->SetStringField(TEXT("Language"), Display.Language);
	DisplayObj->SetBoolField(TEXT("bSubtitleEnabled"), Display.bSubtitleEnabled);
	RootObj->SetObjectField(TEXT("Display"), DisplayObj);

	TSharedPtr<FJsonObject> PrivObj = MakeShareable(new FJsonObject);
	PrivObj->SetBoolField(TEXT("bHidePlayerID"), AccountPrivacy.bHidePlayerID);
	PrivObj->SetBoolField(TEXT("bShareLocationOnline"), AccountPrivacy.bShareLocationOnline);
	PrivObj->SetBoolField(TEXT("bAutoCloudSync"), AccountPrivacy.bAutoCloudSync);
	PrivObj->SetBoolField(TEXT("bAutoLogin"), AccountPrivacy.bAutoLogin);
	RootObj->SetObjectField(TEXT("AccountPrivacy"), PrivObj);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(RootObj.ToSharedRef(), Writer);
	FFileHelper::SaveStringToFile(OutputString, *SettingsFilePath);
}

void UGZSystemSettings::LoadAllSettings()
{
	FString JsonString;
	if (FFileHelper::LoadFileToString(JsonString, *SettingsFilePath))
	{
		TSharedPtr<FJsonObject> RootObj;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
		if (FJsonSerializer::Deserialize(Reader, RootObj) && RootObj.IsValid())
		{
			if (RootObj->HasField(TEXT("Graphics")))
			{
				TSharedPtr<FJsonObject> G = RootObj->GetObjectField(TEXT("Graphics"));
				Graphics.QualityPreset = (EGZQualityPreset)G->GetIntegerField(TEXT("QualityPreset"));
				Graphics.LightPrecision = G->GetIntegerField(TEXT("LightPrecision"));
				Graphics.GlassRefractionSamples = G->GetIntegerField(TEXT("GlassRefractionSamples"));
				Graphics.bEnableTSR = G->GetBoolField(TEXT("bEnableTSR"));
				Graphics.TSRQuality = G->GetIntegerField(TEXT("TSRQuality"));
				Graphics.ResolutionScale = G->GetNumberField(TEXT("ResolutionScale"));
				Graphics.bEnableRainEffects = G->GetBoolField(TEXT("bEnableRainEffects"));
				Graphics.bEnableVegetationWind = G->GetBoolField(TEXT("bEnableVegetationWind"));
				Graphics.bEnableCellBlend = G->GetBoolField(TEXT("bEnableCellBlend"));
				Graphics.bEnableMultiLayerGlass = G->GetBoolField(TEXT("bEnableMultiLayerGlass"));
				Graphics.Brightness = G->GetNumberField(TEXT("Brightness"));
				Graphics.Contrast = G->GetNumberField(TEXT("Contrast"));
				Graphics.ColorTemperature = G->GetNumberField(TEXT("ColorTemperature"));
				Graphics.FieldOfView = G->GetNumberField(TEXT("FieldOfView"));
				Graphics.bEnableRayTracing = G->GetBoolField(TEXT("bEnableRayTracing"));
				Graphics.bEnableRTGI = G->GetBoolField(TEXT("bEnableRTGI"));
				Graphics.bEnableRTShadows = G->GetBoolField(TEXT("bEnableRTShadows"));
				Graphics.bEnableRTReflections = G->GetBoolField(TEXT("bEnableRTReflections"));
				Graphics.bEnableRTAO = G->GetBoolField(TEXT("bEnableRTAO"));
				Graphics.bEnableDLSS = G->GetBoolField(TEXT("bEnableDLSS"));
				Graphics.bEnableFSR = G->GetBoolField(TEXT("bEnableFSR"));
				Graphics.bEnableFrameGen = G->GetBoolField(TEXT("bEnableFrameGen"));
				Graphics.DLSSQuality = G->GetIntegerField(TEXT("DLSSQuality"));
				Graphics.FSRQuality = G->GetIntegerField(TEXT("FSRQuality"));
			}

			if (RootObj->HasField(TEXT("Performance")))
			{
				TSharedPtr<FJsonObject> P = RootObj->GetObjectField(TEXT("Performance"));
				Performance.FrameRateLimit = (EGZFrameRateLimit)P->GetIntegerField(TEXT("FrameRateLimit"));
				Performance.BackgroundFrameRateLimit = P->GetIntegerField(TEXT("BackgroundFrameRateLimit"));
				Performance.PreloadChunkCount = P->GetIntegerField(TEXT("PreloadChunkCount"));
				Performance.NaniteQuality = P->GetIntegerField(TEXT("NaniteQuality"));
				Performance.LumenBounces = P->GetIntegerField(TEXT("LumenBounces"));
				Performance.bEnableTemporalSuperSampling = P->GetBoolField(TEXT("bEnableTemporalSuperSampling"));
				Performance.AdaptiveResolutionTransitionSpeed = P->GetNumberField(TEXT("AdaptiveResolutionTransitionSpeed"));
			}

			if (RootObj->HasField(TEXT("Audio")))
			{
				TSharedPtr<FJsonObject> A = RootObj->GetObjectField(TEXT("Audio"));
				Audio.MasterVolume = A->GetNumberField(TEXT("MasterVolume"));
				Audio.MusicVolume = A->GetNumberField(TEXT("MusicVolume"));
				Audio.SFXVolume = A->GetNumberField(TEXT("SFXVolume"));
				Audio.WeatherSFXVolume = A->GetNumberField(TEXT("WeatherSFXVolume"));
				Audio.VehicleVolume = A->GetNumberField(TEXT("VehicleVolume"));
				Audio.bEnable3DSpatialAudio = A->GetBoolField(TEXT("bEnable3DSpatialAudio"));
			}

			if (RootObj->HasField(TEXT("Controls")))
			{
				TSharedPtr<FJsonObject> C = RootObj->GetObjectField(TEXT("Controls"));
				Controls.MouseSensitivity = C->GetNumberField(TEXT("MouseSensitivity"));
				Controls.ControllerSensitivity = C->GetNumberField(TEXT("ControllerSensitivity"));
				Controls.bInvertYAxis = C->GetBoolField(TEXT("bInvertYAxis"));
				Controls.ViewSmoothing = C->GetNumberField(TEXT("ViewSmoothing"));
				Controls.bEnableMotionBlur = C->GetBoolField(TEXT("bEnableMotionBlur"));
			}

			if (RootObj->HasField(TEXT("Display")))
			{
				TSharedPtr<FJsonObject> D = RootObj->GetObjectField(TEXT("Display"));
				Display.DisplayMode = (EGZDisplayMode)D->GetIntegerField(TEXT("DisplayMode"));
				Display.Resolution.X = D->GetIntegerField(TEXT("ResolutionX"));
				Display.Resolution.Y = D->GetIntegerField(TEXT("ResolutionY"));
				Display.bVSync = D->GetBoolField(TEXT("bVSync"));
				Display.bShowFPS = D->GetBoolField(TEXT("bShowFPS"));
				Display.Language = D->GetStringField(TEXT("Language"));
				Display.bSubtitleEnabled = D->GetBoolField(TEXT("bSubtitleEnabled"));
			}

			ApplyAllSettingsToEngine();
		}
	}
}

void UGZSystemSettings::ResetAllToDefaults()
{
	Graphics = FGZGraphicsSettings();
	Performance = FGZPerformanceSettings();
	Audio = FGZAudioSettings();
	Controls = FGZControlSettings();
	Display = FGZDisplaySettings();
	AccountPrivacy = FGZAccountPrivacySettings();
	Saves = FGZSaveSettings();
	ApplyAllSettingsToEngine();
	SaveAllSettings();
}

// ===== Apply to Engine =====
void UGZSystemSettings::ApplyAllSettingsToEngine()
{
	ApplyGraphicsSettings();
	ApplyPerformanceSettings();
	ApplyAudioSettings();
	ApplyControlSettings();
	ApplyDisplaySettings();
}

static void SafeSetCVar(const TCHAR* Name, float Value)
{
	if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(Name))
	{
		CVar->Set(Value, EConsoleVariableFlags::ECVF_SetByGameSetting);
	}
}

static void SafeSetCVarInt(const TCHAR* Name, int32 Value)
{
	if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(Name))
	{
		CVar->Set(Value, EConsoleVariableFlags::ECVF_SetByGameSetting);
	}
}

void UGZSystemSettings::ApplyGraphicsSettings()
{
	SafeSetCVar(TEXT("r.ScreenPercentage"), Graphics.ResolutionScale);
	SafeSetCVarInt(TEXT("r.TSR.Enable"), Graphics.bEnableTSR ? 1 : 0);
	SafeSetCVarInt(TEXT("r.TSR.History.ScreenPercentage"), Graphics.bEnableTSR ? 200 : 100);
	SafeSetCVarInt(TEXT("r.Lumen.Reflections.MaxBounces"), Graphics.LightPrecision > 1024 ? 6 : 3);
	SafeSetCVarInt(TEXT("r.Glass.MaxLayers"), Graphics.bEnableMultiLayerGlass ? 3 : 1);
	SafeSetCVar(TEXT("r.AmbientLighting.EdgeContrastRetention"), Graphics.Contrast * 0.7f);
	SafeSetCVar(TEXT("r.TonemapperGamma"), Graphics.Brightness);
	SafeSetCVar(TEXT("r.Vegetation.WindStrength"), Graphics.bEnableVegetationWind ? 1.0f : 0.0f);
	SafeSetCVarInt(TEXT("r.Rain.Enabled"), Graphics.bEnableRainEffects ? 1 : 0);
	SafeSetCVarInt(TEXT("r.Lumen.ScreenProbeGather.RadianceCache.NumProbesToTraceBudget"), Graphics.LightPrecision > 1024 ? 300 : 150);

	// macOS / Metal: DLSS/FSR are not available on Apple Silicon; use MetalFX upscaling instead.
	SafeSetCVarInt(TEXT("r.MetalFX.Upscaling.Enable"), (Graphics.bEnableTSR && !Graphics.bEnableDLSS && !Graphics.bEnableFSR) ? 1 : 0);
	SafeSetCVarInt(TEXT("r.MetalFX.FrameInterpolation.Enable"), Graphics.bEnableFrameGen ? 1 : 0);
	SafeSetCVarInt(TEXT("r.RayTracing.Enable"), Graphics.bEnableRayTracing ? 1 : 0);
	SafeSetCVarInt(TEXT("r.Lumen.Reflections.Allow"), Graphics.bEnableRTReflections ? 1 : 0);
	SafeSetCVarInt(TEXT("r.Lumen.ScreenProbeGather.ShortRangeAO"), Graphics.bEnableRTAO ? 1 : 0);

	if (UGameViewportClient* Viewport = GEngine ? GEngine->GameViewport : nullptr)
	{
		Viewport->ViewFOV = Graphics.FieldOfView;
	}
}

void UGZSystemSettings::ApplyPerformanceSettings()
{
	SafeSetCVar(TEXT("r.Nanite.MaxPixelsPerEdge"), (float)Performance.NaniteQuality);
	SafeSetCVarInt(TEXT("r.WorldPartition.PreloadChunkCount"), Performance.PreloadChunkCount);
	SafeSetCVarInt(TEXT("r.Lumen.MaxBounces"), FMath::Clamp(Performance.LumenBounces, 2, 8));
	SafeSetCVarInt(TEXT("r.TSR.History.SampleCount"), Performance.bEnableTemporalSuperSampling ? 16 : 8);
	SafeSetCVar(TEXT("r.TemporalAA.Algorithm"), Performance.bEnableTemporalSuperSampling ? 1.0f : 0.0f);

	if (UGameUserSettings* UserSettings = GEngine ? GEngine->GetGameUserSettings() : nullptr)
	{
		switch (Performance.FrameRateLimit)
		{
		case EGZFrameRateLimit::Limit30: UserSettings->SetFrameRateLimit(30.0f); break;
		case EGZFrameRateLimit::Limit60: UserSettings->SetFrameRateLimit(60.0f); break;
		default: UserSettings->SetFrameRateLimit(0.0f); break;
		}
		UserSettings->ApplySettings(false);
	}
}

void UGZSystemSettings::ApplyAudioSettings()
{
	SafeSetCVar(TEXT("r.Audio.MasterVolume"), Audio.MasterVolume);
	SafeSetCVar(TEXT("r.Audio.MusicVolume"), Audio.MusicVolume);
	SafeSetCVar(TEXT("r.Audio.SFXVolume"), Audio.SFXVolume);
	SafeSetCVar(TEXT("r.Audio.WeatherVolume"), Audio.WeatherSFXVolume);
	SafeSetCVar(TEXT("r.Audio.VehicleVolume"), Audio.VehicleVolume);
	SafeSetCVarInt(TEXT("r.Audio.SpatialAudio"), Audio.bEnable3DSpatialAudio ? 1 : 0);
	SafeSetCVar(TEXT("r.Audio.ReverbIntensity"), Audio.ReverbIntensity);
	SafeSetCVarInt(TEXT("r.Audio.DolbyAtmos"), Audio.bEnableDolbyAtmos ? 1 : 0);
	SafeSetCVarInt(TEXT("r.Audio.3DHeadphones"), Audio.bEnable3DAudioHeadphones ? 1 : 0);
}

void UGZSystemSettings::ApplyControlSettings()
{
	SafeSetCVarInt(TEXT("r.MotionBlur.Enable"), Controls.bEnableMotionBlur ? 1 : 0);
	SafeSetCVar(TEXT("r.MotionBlur.Intensity"), Controls.MotionBlurIntensity);
	SafeSetCVar(TEXT("r.Input.MouseSensitivity"), Controls.MouseSensitivity);
	SafeSetCVar(TEXT("r.Input.ControllerSensitivity"), Controls.ControllerSensitivity);
	SafeSetCVarInt(TEXT("r.Input.InvertY"), Controls.bInvertYAxis ? 1 : 0);
	SafeSetCVar(TEXT("r.Input.ViewSmoothing"), Controls.ViewSmoothing);
}

void UGZSystemSettings::ApplyDisplaySettings()
{
	if (UGameUserSettings* UserSettings = GEngine ? GEngine->GetGameUserSettings() : nullptr)
	{
		switch (Display.DisplayMode)
		{
		case EGZDisplayMode::Fullscreen:			UserSettings->SetFullscreenMode(EWindowMode::Fullscreen); break;
		case EGZDisplayMode::Windowed:			UserSettings->SetFullscreenMode(EWindowMode::Windowed); break;
		case EGZDisplayMode::BorderlessWindowed:	UserSettings->SetFullscreenMode(EWindowMode::WindowedFullscreen); break;
		}

		UserSettings->SetScreenResolution(Display.Resolution);
		UserSettings->SetVSyncEnabled(Display.bVSync);
		UserSettings->ApplySettings(false);
	}

	SafeSetCVarInt(TEXT("r.ShowFPS"), Display.bShowFPS ? 1 : 0);
	SafeSetCVarInt(TEXT("r.Subtitles.Enabled"), Display.bSubtitleEnabled ? 1 : 0);

	FInternationalization::Get().SetCurrentCulture(Display.Language);
}

// ===== Individual setters =====
void UGZSystemSettings::SetResolutionScale(float Scale) { Graphics.ResolutionScale = FMath::Clamp(Scale, 70.0f, 100.0f); ApplyGraphicsSettings(); SaveAllSettings(); }
void UGZSystemSettings::SetBrightness(float Brightness) { Graphics.Brightness = FMath::Clamp(Brightness, 0.5f, 2.0f); ApplyGraphicsSettings(); SaveAllSettings(); }
void UGZSystemSettings::SetContrast(float Contrast) { Graphics.Contrast = FMath::Clamp(Contrast, 0.5f, 2.0f); ApplyGraphicsSettings(); SaveAllSettings(); }
void UGZSystemSettings::SetColorTemperature(float Temp) { Graphics.ColorTemperature = FMath::Clamp(Temp, 1000.0f, 15000.0f); ApplyGraphicsSettings(); SaveAllSettings(); }
void UGZSystemSettings::SetFieldOfView(float FOV) { Graphics.FieldOfView = FMath::Clamp(FOV, 60.0f, 120.0f); ApplyGraphicsSettings(); SaveAllSettings(); }
void UGZSystemSettings::ToggleRainEffects(bool bEnabled) { Graphics.bEnableRainEffects = bEnabled; ApplyGraphicsSettings(); SaveAllSettings(); }
void UGZSystemSettings::ToggleVegetationWind(bool bEnabled) { Graphics.bEnableVegetationWind = bEnabled; ApplyGraphicsSettings(); SaveAllSettings(); }
void UGZSystemSettings::ToggleRayTracing(bool bEnabled) { Graphics.bEnableRayTracing = bEnabled; ApplyGraphicsSettings(); SaveAllSettings(); }
void UGZSystemSettings::ToggleRTGI(bool bEnabled) { Graphics.bEnableRTGI = bEnabled; ApplyGraphicsSettings(); SaveAllSettings(); }
void UGZSystemSettings::ToggleRTShadows(bool bEnabled) { Graphics.bEnableRTShadows = bEnabled; ApplyGraphicsSettings(); SaveAllSettings(); }
void UGZSystemSettings::ToggleRTReflections(bool bEnabled) { Graphics.bEnableRTReflections = bEnabled; ApplyGraphicsSettings(); SaveAllSettings(); }
void UGZSystemSettings::ToggleRTAO(bool bEnabled) { Graphics.bEnableRTAO = bEnabled; ApplyGraphicsSettings(); SaveAllSettings(); }
void UGZSystemSettings::ToggleDLSS(bool bEnabled) { Graphics.bEnableDLSS = bEnabled; ApplyGraphicsSettings(); SaveAllSettings(); }
void UGZSystemSettings::ToggleFSR(bool bEnabled) { Graphics.bEnableFSR = bEnabled; ApplyGraphicsSettings(); SaveAllSettings(); }
void UGZSystemSettings::ToggleFrameGen(bool bEnabled) { Graphics.bEnableFrameGen = bEnabled; ApplyGraphicsSettings(); SaveAllSettings(); }
void UGZSystemSettings::SetFrameRateLimit(EGZFrameRateLimit Limit) { Performance.FrameRateLimit = Limit; ApplyPerformanceSettings(); SaveAllSettings(); }
void UGZSystemSettings::SetPreloadChunkCount(int32 Count) { Performance.PreloadChunkCount = FMath::Clamp(Count, 2, 16); ApplyPerformanceSettings(); SaveAllSettings(); }
void UGZSystemSettings::SetMasterVolume(float Volume) { Audio.MasterVolume = FMath::Clamp(Volume, 0.0f, 1.0f); ApplyAudioSettings(); SaveAllSettings(); }
void UGZSystemSettings::SetMusicVolume(float Volume) { Audio.MusicVolume = FMath::Clamp(Volume, 0.0f, 1.0f); ApplyAudioSettings(); SaveAllSettings(); }
void UGZSystemSettings::SetSFXVolume(float Volume) { Audio.SFXVolume = FMath::Clamp(Volume, 0.0f, 1.0f); ApplyAudioSettings(); SaveAllSettings(); }
void UGZSystemSettings::SetMouseSensitivity(float Sensitivity) { Controls.MouseSensitivity = FMath::Clamp(Sensitivity, 0.1f, 10.0f); ApplyControlSettings(); SaveAllSettings(); }
void UGZSystemSettings::SetInvertY(bool bInvert) { Controls.bInvertYAxis = bInvert; SaveAllSettings(); }
void UGZSystemSettings::SetViewSmoothing(float Smoothness) { Controls.ViewSmoothing = FMath::Clamp(Smoothness, 0.0f, 1.0f); SaveAllSettings(); }
void UGZSystemSettings::SetAutoCloudSync(bool bEnabled) { AccountPrivacy.bAutoCloudSync = bEnabled; SaveAllSettings(); }
void UGZSystemSettings::SetAutoLogin(bool bEnabled) { AccountPrivacy.bAutoLogin = bEnabled; SaveAllSettings(); }
void UGZSystemSettings::SetAutoSaveEnabled(bool bEnabled) { Saves.bAutoSaveEnabled = bEnabled; SaveAllSettings(); }
void UGZSystemSettings::SetAutoSaveInterval(float Minutes) { Saves.AutoSaveIntervalMinutes = FMath::Clamp(Minutes, 5.0f, 60.0f); SaveAllSettings(); }
void UGZSystemSettings::SetDisplayMode(EGZDisplayMode Mode) { Display.DisplayMode = Mode; ApplyDisplaySettings(); SaveAllSettings(); }
void UGZSystemSettings::SetResolution(const FIntPoint& Resolution) { Display.Resolution = Resolution; ApplyDisplaySettings(); SaveAllSettings(); }
void UGZSystemSettings::SetVSync(bool bEnabled) { Display.bVSync = bEnabled; ApplyDisplaySettings(); SaveAllSettings(); }
void UGZSystemSettings::SetShowFPS(bool bShow) { Display.bShowFPS = bShow; ApplyDisplaySettings(); SaveAllSettings(); }
void UGZSystemSettings::SetLanguage(const FString& Language) { Display.Language = Language; ApplyDisplaySettings(); SaveAllSettings(); }
void UGZSystemSettings::SetSubtitleEnabled(bool bEnabled) { Display.bSubtitleEnabled = bEnabled; ApplyDisplaySettings(); SaveAllSettings(); }
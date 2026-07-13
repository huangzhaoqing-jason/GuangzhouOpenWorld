#include "Game/GZSettingsManager.h"
#include "GuangzhouOpenWorld.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonReader.h"
#include "Dom/JsonObject.h"
#include "GameFramework/GameUserSettings.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

UGZSettingsManager::UGZSettingsManager()
{
	SettingsFilePath = FPaths::ProjectSavedDir() / TEXT("GameSettings.json");
}

void UGZSettingsManager::LoadSettings()
{
	FString JsonString;
	if (FFileHelper::LoadFileToString(JsonString, *SettingsFilePath))
	{
		TSharedPtr<FJsonObject> RootObj;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
		if (FJsonSerializer::Deserialize(Reader, RootObj) && RootObj.IsValid())
		{
			CurrentSettings.MasterVolume = RootObj->GetNumberField(TEXT("MasterVolume"));
			CurrentSettings.MusicVolume = RootObj->GetNumberField(TEXT("MusicVolume"));
			CurrentSettings.SFXVolume = RootObj->GetNumberField(TEXT("SFXVolume"));
			CurrentSettings.DialogueVolume = RootObj->GetNumberField(TEXT("DialogueVolume"));
			CurrentSettings.MouseSensitivity = RootObj->GetNumberField(TEXT("MouseSensitivity"));
			CurrentSettings.ControllerSensitivity = RootObj->GetNumberField(TEXT("ControllerSensitivity"));
			CurrentSettings.bInvertY = RootObj->GetBoolField(TEXT("bInvertY"));
			CurrentSettings.GraphicsQuality = RootObj->GetIntegerField(TEXT("GraphicsQuality"));
			CurrentSettings.ResolutionScale = RootObj->GetIntegerField(TEXT("ResolutionScale"));
			CurrentSettings.bVSync = RootObj->GetBoolField(TEXT("bVSync"));
			CurrentSettings.bShowFPS = RootObj->GetBoolField(TEXT("bShowFPS"));
			CurrentSettings.Language = RootObj->GetStringField(TEXT("Language"));
			CurrentSettings.bSubtitleEnabled = RootObj->GetBoolField(TEXT("bSubtitleEnabled"));
			CurrentSettings.FOV = RootObj->GetNumberField(TEXT("FOV"));
			CurrentSettings.Brightness = RootObj->GetNumberField(TEXT("Brightness"));

			ApplySettings();
			UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Settings loaded from %s"), *SettingsFilePath);
			return;
		}
	}

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("No settings file found, using defaults"));
}

void UGZSettingsManager::SaveSettings()
{
	TSharedPtr<FJsonObject> RootObj = MakeShareable(new FJsonObject);
	RootObj->SetNumberField(TEXT("MasterVolume"), CurrentSettings.MasterVolume);
	RootObj->SetNumberField(TEXT("MusicVolume"), CurrentSettings.MusicVolume);
	RootObj->SetNumberField(TEXT("SFXVolume"), CurrentSettings.SFXVolume);
	RootObj->SetNumberField(TEXT("DialogueVolume"), CurrentSettings.DialogueVolume);
	RootObj->SetNumberField(TEXT("MouseSensitivity"), CurrentSettings.MouseSensitivity);
	RootObj->SetNumberField(TEXT("ControllerSensitivity"), CurrentSettings.ControllerSensitivity);
	RootObj->SetBoolField(TEXT("bInvertY"), CurrentSettings.bInvertY);
	RootObj->SetIntegerField(TEXT("GraphicsQuality"), CurrentSettings.GraphicsQuality);
	RootObj->SetIntegerField(TEXT("ResolutionScale"), CurrentSettings.ResolutionScale);
	RootObj->SetBoolField(TEXT("bVSync"), CurrentSettings.bVSync);
	RootObj->SetBoolField(TEXT("bShowFPS"), CurrentSettings.bShowFPS);
	RootObj->SetStringField(TEXT("Language"), CurrentSettings.Language);
	RootObj->SetBoolField(TEXT("bSubtitleEnabled"), CurrentSettings.bSubtitleEnabled);
	RootObj->SetNumberField(TEXT("FOV"), CurrentSettings.FOV);
	RootObj->SetNumberField(TEXT("Brightness"), CurrentSettings.Brightness);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(RootObj.ToSharedRef(), Writer);
	FFileHelper::SaveStringToFile(OutputString, *SettingsFilePath);
}

void UGZSettingsManager::ResetToDefaults()
{
	CurrentSettings = FGZSettingsSaveData();
	SaveSettings();
	ApplySettings();
}

void UGZSettingsManager::ApplySettings()
{
	if (UGameUserSettings* UserSettings = GEngine ? GEngine->GetGameUserSettings() : nullptr)
	{
		UserSettings->SetFullscreenMode(EWindowMode::Fullscreen);
		UserSettings->SetVSyncEnabled(CurrentSettings.bVSync);
		UserSettings->SetScreenPercentage(CurrentSettings.ResolutionScale);
		UserSettings->SetOverallScalabilityLevel(CurrentSettings.GraphicsQuality);
		UserSettings->ApplySettings(false);
	}
}

void UGZSettingsManager::SetMasterVolume(float Volume) { CurrentSettings.MasterVolume = FMath::Clamp(Volume, 0.0f, 1.0f); SaveSettings(); }
void UGZSettingsManager::SetMusicVolume(float Volume) { CurrentSettings.MusicVolume = FMath::Clamp(Volume, 0.0f, 1.0f); SaveSettings(); }
void UGZSettingsManager::SetSFXVolume(float Volume) { CurrentSettings.SFXVolume = FMath::Clamp(Volume, 0.0f, 1.0f); SaveSettings(); }
void UGZSettingsManager::SetDialogueVolume(float Volume) { CurrentSettings.DialogueVolume = FMath::Clamp(Volume, 0.0f, 1.0f); SaveSettings(); }
void UGZSettingsManager::SetMouseSensitivity(float Sensitivity) { CurrentSettings.MouseSensitivity = FMath::Clamp(Sensitivity, 0.1f, 10.0f); SaveSettings(); }
void UGZSettingsManager::SetControllerSensitivity(float Sensitivity) { CurrentSettings.ControllerSensitivity = FMath::Clamp(Sensitivity, 0.1f, 10.0f); SaveSettings(); }
void UGZSettingsManager::SetInvertY(bool bInvert) { CurrentSettings.bInvertY = bInvert; SaveSettings(); }
void UGZSettingsManager::SetGraphicsQuality(int32 Quality) { CurrentSettings.GraphicsQuality = FMath::Clamp(Quality, 0, 4); SaveSettings(); ApplySettings(); }
void UGZSettingsManager::SetResolutionScale(int32 Scale) { CurrentSettings.ResolutionScale = FMath::Clamp(Scale, 50, 200); SaveSettings(); ApplySettings(); }
void UGZSettingsManager::SetVSync(bool bEnable) { CurrentSettings.bVSync = bEnable; SaveSettings(); ApplySettings(); }
void UGZSettingsManager::SetShowFPS(bool bShow) { CurrentSettings.bShowFPS = bShow; SaveSettings(); }
void UGZSettingsManager::SetFOV(float NewFOV) { CurrentSettings.FOV = FMath::Clamp(NewFOV, 60.0f, 120.0f); SaveSettings(); }
void UGZSettingsManager::SetBrightness(float NewBrightness) { CurrentSettings.Brightness = FMath::Clamp(NewBrightness, 0.5f, 2.0f); SaveSettings(); }
void UGZSettingsManager::SetLanguage(const FString& Language) { CurrentSettings.Language = Language; SaveSettings(); }
void UGZSettingsManager::SetSubtitleEnabled(bool bEnabled) { CurrentSettings.bSubtitleEnabled = bEnabled; SaveSettings(); }
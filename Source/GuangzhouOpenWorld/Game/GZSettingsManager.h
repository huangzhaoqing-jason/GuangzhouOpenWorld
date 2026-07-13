#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZSaveGame.h"
#include "GZSettingsManager.generated.h"

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZSettingsManager : public UObject
{
	GENERATED_BODY()

public:
	UGZSettingsManager();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void LoadSettings();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SaveSettings();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ResetToDefaults();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ApplySettings();

	UFUNCTION(BlueprintPure, Category = "Settings")
	const FGZSettingsSaveData& GetCurrentSettings() const { return CurrentSettings; }

	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void SetMasterVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void SetMusicVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void SetSFXVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void SetDialogueVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "Settings|Controls")
	void SetMouseSensitivity(float Sensitivity);

	UFUNCTION(BlueprintCallable, Category = "Settings|Controls")
	void SetControllerSensitivity(float Sensitivity);

	UFUNCTION(BlueprintCallable, Category = "Settings|Controls")
	void SetInvertY(bool bInvert);

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetGraphicsQuality(int32 Quality);

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetResolutionScale(int32 Scale);

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetVSync(bool bEnable);

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetShowFPS(bool bShow);

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetFOV(float NewFOV);

	UFUNCTION(BlueprintCallable, Category = "Settings|Graphics")
	void SetBrightness(float NewBrightness);

	UFUNCTION(BlueprintCallable, Category = "Settings|Display")
	void SetLanguage(const FString& Language);

	UFUNCTION(BlueprintCallable, Category = "Settings|Display")
	void SetSubtitleEnabled(bool bEnabled);

private:
	FGZSettingsSaveData CurrentSettings;
	FString SettingsFilePath;
};
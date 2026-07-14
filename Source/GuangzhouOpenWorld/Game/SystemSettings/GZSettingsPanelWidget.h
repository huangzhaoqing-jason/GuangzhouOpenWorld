#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SystemSettings/GZSystemSettings.h"
#include "GZSettingsPanelWidget.generated.h"

UCLASS()
class GUANGZHOUOPENWORLD_API UGZSettingsPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UGZSettingsPanelWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void SwitchCategory(EGZSettingsCategory Category);

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void ApplyCurrentSettings();

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void ResetToDefaults();

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void ReturnToPreviousMenu();

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void SetQualityPreset(EGZQualityPreset Preset);

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void SetResolutionScale(float Scale);

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void SetBrightness(float Brightness);

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void SetContrast(float Contrast);

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void SetFieldOfView(float FOV);

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void ToggleRainEffects(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void ToggleVegetationWind(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void ToggleRayTracing(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void ToggleRTGI(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void ToggleRTReflections(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void ToggleFrameGen(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void SetFrameRateLimit(EGZFrameRateLimit Limit);

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void SetPreloadChunkCount(int32 Count);

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void SetMasterVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void SetMusicVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void SetSFXVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void SetMouseSensitivity(float Sensitivity);

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void SetInvertY(bool bInvert);

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void SetAutoCloudSync(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void SetAutoSaveEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void SetDisplayMode(EGZDisplayMode Mode);

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void SetResolution(const FIntPoint& Resolution);

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void SetVSync(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void SetShowFPS(bool bShow);

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void SetLanguage(const FString& Language);

	UFUNCTION(BlueprintCallable, Category = "SettingsPanel")
	void SetSubtitleEnabled(bool bEnabled);

	UFUNCTION(BlueprintPure, Category = "SettingsPanel")
	EGZSettingsCategory GetCurrentCategory() const { return CurrentCategory; }

	UFUNCTION(BlueprintPure, Category = "SettingsPanel")
	UGZSystemSettings* GetSystemSettings() const;

	UFUNCTION(BlueprintPure, Category = "SettingsPanel")
	FText GetCategoryDisplayName(EGZSettingsCategory Category) const;

	UPROPERTY(BlueprintReadOnly, Category = "SettingsPanel")
	bool bPendingRestart = false;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "SettingsPanel")
	EGZSettingsCategory CurrentCategory = EGZSettingsCategory::Graphics;

	void RefreshAllUI();
};

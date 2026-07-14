#include "Game/SystemSettings/GZSettingsPanelWidget.h"
#include "Game/GZGameInstance.h"
#include "Game/GZSaveGameManager.h"
#include "GuangzhouOpenWorld.h"

UGZSettingsPanelWidget::UGZSettingsPanelWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGZSettingsPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();
	RefreshAllUI();
}

void UGZSettingsPanelWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UGZSettingsPanelWidget::SwitchCategory(EGZSettingsCategory Category)
{
	CurrentCategory = Category;
	RefreshAllUI();
}

void UGZSettingsPanelWidget::ApplyCurrentSettings()
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->ApplyAllSettingsToEngine();
		Settings->SaveAllSettings();
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Settings applied and saved"));
	}
}

void UGZSettingsPanelWidget::ResetToDefaults()
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->ResetAllToDefaults();
		RefreshAllUI();
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Settings reset to defaults"));
	}
}

void UGZSettingsPanelWidget::ReturnToPreviousMenu()
{
	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	if (!GI) return;

	if (GI->IsPlaying())
	{
		GI->SetGameState(EGZGameState::Playing);
	}
	else
	{
		GI->SetGameState(EGZGameState::MainMenu);
	}
}

void UGZSettingsPanelWidget::SetQualityPreset(EGZQualityPreset Preset)
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->SetQualityPreset(Preset);
		bPendingRestart = (Preset == EGZQualityPreset::Ultra);
	}
}

void UGZSettingsPanelWidget::SetResolutionScale(float Scale)
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->SetResolutionScale(Scale);
	}
}

void UGZSettingsPanelWidget::SetBrightness(float Brightness)
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->SetBrightness(Brightness);
	}
}

void UGZSettingsPanelWidget::SetContrast(float Contrast)
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->SetContrast(Contrast);
	}
}

void UGZSettingsPanelWidget::SetFieldOfView(float FOV)
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->SetFieldOfView(FOV);
	}
}

void UGZSettingsPanelWidget::ToggleRainEffects(bool bEnabled)
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->ToggleRainEffects(bEnabled);
	}
}

void UGZSettingsPanelWidget::ToggleVegetationWind(bool bEnabled)
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->ToggleVegetationWind(bEnabled);
	}
}

void UGZSettingsPanelWidget::ToggleRayTracing(bool bEnabled)
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->ToggleRayTracing(bEnabled);
	}
}

void UGZSettingsPanelWidget::ToggleRTGI(bool bEnabled)
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->ToggleRTGI(bEnabled);
	}
}

void UGZSettingsPanelWidget::ToggleRTReflections(bool bEnabled)
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->ToggleRTReflections(bEnabled);
	}
}

void UGZSettingsPanelWidget::ToggleFrameGen(bool bEnabled)
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->ToggleFrameGen(bEnabled);
	}
}

void UGZSettingsPanelWidget::SetFrameRateLimit(EGZFrameRateLimit Limit)
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->SetFrameRateLimit(Limit);
	}
}

void UGZSettingsPanelWidget::SetPreloadChunkCount(int32 Count)
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->SetPreloadChunkCount(Count);
	}
}

void UGZSettingsPanelWidget::SetMasterVolume(float Volume)
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->SetMasterVolume(Volume);
	}
}

void UGZSettingsPanelWidget::SetMusicVolume(float Volume)
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->SetMusicVolume(Volume);
	}
}

void UGZSettingsPanelWidget::SetSFXVolume(float Volume)
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->SetSFXVolume(Volume);
	}
}

void UGZSettingsPanelWidget::SetMouseSensitivity(float Sensitivity)
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->SetMouseSensitivity(Sensitivity);
	}
}

void UGZSettingsPanelWidget::SetInvertY(bool bInvert)
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->SetInvertY(bInvert);
	}
}

void UGZSettingsPanelWidget::SetAutoCloudSync(bool bEnabled)
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->SetAutoCloudSync(bEnabled);
	}
}

void UGZSettingsPanelWidget::SetAutoSaveEnabled(bool bEnabled)
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->SetAutoSaveEnabled(bEnabled);
	}

	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	if (GI && GI->GetSaveManager())
	{
		GI->GetSaveManager()->bAutoSaveEnabled = bEnabled;
	}
}

void UGZSettingsPanelWidget::SetDisplayMode(EGZDisplayMode Mode)
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->SetDisplayMode(Mode);
	}
}

void UGZSettingsPanelWidget::SetResolution(const FIntPoint& Resolution)
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->SetResolution(Resolution);
	}
}

void UGZSettingsPanelWidget::SetVSync(bool bEnabled)
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->SetVSync(bEnabled);
	}
}

void UGZSettingsPanelWidget::SetShowFPS(bool bShow)
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->SetShowFPS(bShow);
	}
}

void UGZSettingsPanelWidget::SetLanguage(const FString& Language)
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->SetLanguage(Language);
	}
}

void UGZSettingsPanelWidget::SetSubtitleEnabled(bool bEnabled)
{
	if (UGZSystemSettings* Settings = GetSystemSettings())
	{
		Settings->SetSubtitleEnabled(bEnabled);
	}
}

UGZSystemSettings* UGZSettingsPanelWidget::GetSystemSettings() const
{
	UGZGameInstance* GI = Cast<UGZGameInstance>(GetGameInstance());
	return GI ? GI->GetSystemSettings() : nullptr;
}

FText UGZSettingsPanelWidget::GetCategoryDisplayName(EGZSettingsCategory Category) const
{
	switch (Category)
	{
	case EGZSettingsCategory::Graphics:     return FText::FromString(TEXT("画面"));
	case EGZSettingsCategory::Performance:  return FText::FromString(TEXT("性能"));
	case EGZSettingsCategory::Audio:        return FText::FromString(TEXT("音频"));
	case EGZSettingsCategory::Controls:     return FText::FromString(TEXT("控制"));
	case EGZSettingsCategory::Display:      return FText::FromString(TEXT("显示"));
	case EGZSettingsCategory::Account:      return FText::FromString(TEXT("账号与隐私"));
	case EGZSettingsCategory::Saves:        return FText::FromString(TEXT("存档"));
	default:                                return FText::FromString(TEXT("未知"));
	}
}

void UGZSettingsPanelWidget::RefreshAllUI()
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("SettingsPanel switched to category %d"), (int32)CurrentCategory);
}

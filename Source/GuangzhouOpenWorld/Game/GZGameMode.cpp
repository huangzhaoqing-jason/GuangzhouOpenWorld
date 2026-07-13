#include "Game/GZGameMode.h"
#include "GuangzhouOpenWorld.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Engine/SkyLight.h"
#include "Components/SkyLightComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "HAL/PlatformMisc.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/ExponentialHeightFog.h"
#include "Curves/CurveFloat.h"

AGZGameMode::AGZGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f / 60.0f;
	DayNight.TimeOfDay = 8.0f;
}

void AGZGameMode::BeginPlay()
{
	Super::BeginPlay();

	DetectedChip = DetectAppleSiliconChip();
	ApplyChipSpecificSettings();

	SunLight = Cast<ADirectionalLight>(UGameplayStatics::GetActorOfClass(GetWorld(), ADirectionalLight::StaticClass()));
	if (SunLight)
	{
		SunComponent = SunLight->GetComponent();
	}

	SkyLightActor = Cast<ASkyLight>(UGameplayStatics::GetActorOfClass(GetWorld(), ASkyLight::StaticClass()));
	if (SkyLightActor)
	{
		SkyComponent = SkyLightActor->GetLightComponent();
	}

	ApplyAutoQualityPreset();
	UpdateLightingFromZone(CurrentLightingZone);
}

void AGZGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateDayNightCycle(DeltaSeconds);
	UpdateWeatherTransition(DeltaSeconds);
}

EAppleSiliconChip AGZGameMode::DetectAppleSiliconChip() const
{
#if PLATFORM_MAC
	const FString CPUBrand = FPlatformMisc::GetCPUBrand();
	if (CPUBrand.Contains(TEXT("M3"))) return EAppleSiliconChip::M3;
	if (CPUBrand.Contains(TEXT("M2"))) return EAppleSiliconChip::M2;
	if (CPUBrand.Contains(TEXT("M1"))) return EAppleSiliconChip::M1;
#endif
	return EAppleSiliconChip::Unknown;
}

void AGZGameMode::ApplyChipSpecificSettings()
{
	switch (DetectedChip)
	{
	case EAppleSiliconChip::M1:
		QualitySettings.ScreenPercentage = 75.0f;
		QualitySettings.ShadowResolution = 1024;
		QualitySettings.VolumetricFogGridSize = 96;
		QualitySettings.SSRQuality = 2;
		QualitySettings.BloomIntensity = 0.8f;
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Applied M1 quality profile: Screen%%=75, Shadow=1024, VolFog=96, SSR=2"));
		break;

	case EAppleSiliconChip::M2:
		QualitySettings.ScreenPercentage = 90.0f;
		QualitySettings.ShadowResolution = 1536;
		QualitySettings.VolumetricFogGridSize = 112;
		QualitySettings.SSRQuality = 3;
		QualitySettings.BloomIntensity = 0.9f;
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Applied M2 quality profile: Screen%%=90, Shadow=1536, VolFog=112, SSR=3"));
		break;

	case EAppleSiliconChip::M3:
		QualitySettings.ScreenPercentage = 100.0f;
		QualitySettings.ShadowResolution = 2048;
		QualitySettings.VolumetricFogGridSize = 128;
		QualitySettings.SSRQuality = 4;
		QualitySettings.BloomIntensity = 1.0f;
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Applied M3 quality profile: Screen%%=100, Shadow=2048, VolFog=128, SSR=4"));
		break;

	default:
		QualitySettings.ScreenPercentage = 70.0f;
		QualitySettings.ShadowResolution = 1024;
		QualitySettings.VolumetricFogGridSize = 80;
		QualitySettings.SSRQuality = 2;
		QualitySettings.BloomIntensity = 0.7f;
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Applied default quality profile (Unknown chip)"));
		break;
	}
}

void AGZGameMode::SetWeather(EGZWeatherType NewWeather, float TransitionTime)
{
	if (CurrentWeather == NewWeather && WeatherTransitionProgress >= 1.0f) return;

	TargetWeather = NewWeather;
	WeatherTransitionProgress = 0.0f;
	WeatherTransitionSpeed = TransitionTime > 0.0f ? 1.0f / TransitionTime : 1.0f;
}

void AGZGameMode::SetLightingZone(EGZLightingZone NewZone)
{
	if (CurrentLightingZone == NewZone) return;
	CurrentLightingZone = NewZone;
	UpdateLightingFromZone(NewZone);
}

void AGZGameMode::SetTimeOfDay(float NewTime)
{
	DayNight.TimeOfDay = FMath::Fmod(NewTime, 24.0f);
}

void AGZGameMode::UpdateDayNightCycle(float DeltaSeconds)
{
	DayNight.TimeOfDay += DeltaSeconds * (24.0f / DayNight.DayLengthSeconds);
	if (DayNight.TimeOfDay >= 24.0f) DayNight.TimeOfDay -= 24.0f;

	const float Hour = DayNight.TimeOfDay;
	float SunAngleDeg = (Hour - 6.0f) * 15.0f;
	DayNight.SunAngle = SunAngleDeg;

	float Intensity;
	FLinearColor Color;
	if (Hour >= 5.0f && Hour < 7.0f)
	{
		const float T = (Hour - 5.0f) / 2.0f;
		Intensity = FMath::Lerp(DayNight.NightIntensity, DayNight.SunIntensity, T);
		Color = FMath::Lerp(DayNight.SunriseColor, DayNight.DayColor, T);
	}
	else if (Hour >= 7.0f && Hour < 17.0f)
	{
		Intensity = DayNight.SunIntensity;
		Color = DayNight.DayColor;
	}
	else if (Hour >= 17.0f && Hour < 19.0f)
	{
		const float T = (Hour - 17.0f) / 2.0f;
		Intensity = FMath::Lerp(DayNight.SunIntensity, DayNight.NightIntensity, T);
		Color = FMath::Lerp(DayNight.DayColor, DayNight.SunsetColor, T);
	}
	else
	{
		Intensity = DayNight.NightIntensity;
		Color = DayNight.NightColor;
	}

	if (SunComponent)
	{
		SunComponent->SetIntensity(Intensity);
		SunComponent->SetLightColor(Color);
		FRotator SunRot(-SunAngleDeg + 90.0f, 0.0f, 0.0f);
		SunLight->SetActorRotation(SunRot);
	}

	if (SkyComponent)
	{
		SkyComponent->SetIntensity(FMath::Lerp(0.5f, 2.0f, Intensity / DayNight.SunIntensity));
		SkyComponent->SetLightColor(FMath::Lerp(DayNight.NightColor, DayNight.DayColor, Intensity / DayNight.SunIntensity));
	}
}

void AGZGameMode::UpdateWeatherTransition(float DeltaSeconds)
{
	if (WeatherTransitionProgress >= 1.0f) return;

	WeatherTransitionProgress += WeatherTransitionSpeed * DeltaSeconds;
	if (WeatherTransitionProgress >= 1.0f)
	{
		WeatherTransitionProgress = 1.0f;
		CurrentWeather = TargetWeather;
	}

	AExponentialHeightFog* FogActor = Cast<AExponentialHeightFog>(UGameplayStatics::GetActorOfClass(GetWorld(), AExponentialHeightFog::StaticClass()));
	if (!FogActor) return;

	switch (TargetWeather)
	{
	case EGZWeatherType::Clear:
		FogActor->GetComponent()->SetFogDensity(FMath::Lerp(0.02f, 0.01f, WeatherTransitionProgress));
		break;
	case EGZWeatherType::Cloudy:
		FogActor->GetComponent()->SetFogDensity(FMath::Lerp(0.02f, 0.03f, WeatherTransitionProgress));
		break;
	case EGZWeatherType::Rain:
		FogActor->GetComponent()->SetFogDensity(FMath::Lerp(0.02f, 0.04f, WeatherTransitionProgress));
		break;
	case EGZWeatherType::Storm:
		FogActor->GetComponent()->SetFogDensity(FMath::Lerp(0.02f, 0.06f, WeatherTransitionProgress));
		break;
	case EGZWeatherType::FogHaze:
		FogActor->GetComponent()->SetFogDensity(FMath::Lerp(0.02f, 0.08f, WeatherTransitionProgress));
		break;
	}
}

void AGZGameMode::UpdateLightingFromZone(EGZLightingZone Zone)
{
	APostProcessVolume* PPV = Cast<APostProcessVolume>(UGameplayStatics::GetActorOfClass(GetWorld(), APostProcessVolume::StaticClass()));
	if (!PPV) return;

	switch (Zone)
	{
	case EGZLightingZone::OutdoorStreet:
		PPV->Settings.bOverride_AutoExposureBias = true;
		PPV->Settings.AutoExposureBias = 0.0f;
		PPV->Settings.bOverride_ColorGradingIntensity = true;
		PPV->Settings.ColorGradingIntensity = 1.0f;
		break;

	case EGZLightingZone::Indoor:
		PPV->Settings.bOverride_AutoExposureBias = true;
		PPV->Settings.AutoExposureBias = 1.5f;
		PPV->Settings.bOverride_ColorGradingIntensity = true;
		PPV->Settings.ColorGradingIntensity = 1.1f;
		break;

	case EGZLightingZone::Tunnel:
		PPV->Settings.bOverride_AutoExposureBias = true;
		PPV->Settings.AutoExposureBias = 2.0f;
		PPV->Settings.bOverride_ColorGradingIntensity = true;
		PPV->Settings.ColorGradingIntensity = 0.9f;
		break;

	case EGZLightingZone::RiverSurface:
		PPV->Settings.bOverride_AutoExposureBias = true;
		PPV->Settings.AutoExposureBias = -0.5f;
		PPV->Settings.bOverride_ColorGradingIntensity = true;
		PPV->Settings.ColorGradingIntensity = 1.2f;
		break;
	}
}

void AGZGameMode::ApplyAutoQualityPreset()
{
	if (const auto* RendererSettings = GetDefault<URendererSettings>())
	{
	}
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Auto quality preset applied: Screen%%=%.0f, Shadow=%d, VolFog=%d, SSR=%d, Bloom=%.1f"),
		QualitySettings.ScreenPercentage, QualitySettings.ShadowResolution,
		QualitySettings.VolumetricFogGridSize, QualitySettings.SSRQuality, QualitySettings.BloomIntensity);
}
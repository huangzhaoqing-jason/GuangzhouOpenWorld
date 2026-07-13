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
#include "Components/ExponentialHeightFogComponent.h"
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

	CausalitySchedule.Empty();
	CausalitySchedule.Reserve(7);

	{
		FDayNightCausalityEntry Entry;
		Entry.HourStart = 6.0f;
		Entry.HourEnd = 8.0f;
		Entry.ColorTempKelvin = 6500.0f;
		Entry.ShadowLengthMultiplier = 3.8f;
		Entry.ShadowSoftness = 0.9f;
		Entry.LightColor = FLinearColor(0.85f, 0.88f, 1.0f, 1.0f);
		Entry.PeriodName = TEXT("Dawn");
		CausalitySchedule.Add(Entry);
	}
	{
		FDayNightCausalityEntry Entry;
		Entry.HourStart = 8.0f;
		Entry.HourEnd = 11.0f;
		Entry.ColorTempKelvin = 5800.0f;
		Entry.ShadowLengthMultiplier = 2.2f;
		Entry.ShadowSoftness = 0.5f;
		Entry.LightColor = FLinearColor(1.0f, 0.98f, 0.95f, 1.0f);
		Entry.PeriodName = TEXT("Morning");
		CausalitySchedule.Add(Entry);
	}
	{
		FDayNightCausalityEntry Entry;
		Entry.HourStart = 11.0f;
		Entry.HourEnd = 14.0f;
		Entry.ColorTempKelvin = 5200.0f;
		Entry.ShadowLengthMultiplier = 0.9f;
		Entry.ShadowSoftness = 0.1f;
		Entry.LightColor = FLinearColor(1.0f, 0.95f, 0.90f, 1.0f);
		Entry.PeriodName = TEXT("Midday");
		CausalitySchedule.Add(Entry);
	}
	{
		FDayNightCausalityEntry Entry;
		Entry.HourStart = 14.0f;
		Entry.HourEnd = 17.0f;
		Entry.ColorTempKelvin = 5600.0f;
		Entry.ShadowLengthMultiplier = 2.0f;
		Entry.ShadowSoftness = 0.4f;
		Entry.LightColor = FLinearColor(1.0f, 0.92f, 0.78f, 1.0f);
		Entry.PeriodName = TEXT("Afternoon");
		CausalitySchedule.Add(Entry);
	}
	{
		FDayNightCausalityEntry Entry;
		Entry.HourStart = 17.0f;
		Entry.HourEnd = 19.0f;
		Entry.ColorTempKelvin = 3200.0f;
		Entry.ShadowLengthMultiplier = 4.2f;
		Entry.ShadowSoftness = 0.95f;
		Entry.LightColor = FLinearColor(1.0f, 0.55f, 0.25f, 1.0f);
		Entry.PeriodName = TEXT("Dusk");
		CausalitySchedule.Add(Entry);
	}
	{
		FDayNightCausalityEntry Entry;
		Entry.HourStart = 19.0f;
		Entry.HourEnd = 6.0f;
		Entry.ColorTempKelvin = 18000.0f;
		Entry.ShadowLengthMultiplier = 0.0f;
		Entry.ShadowSoftness = 1.0f;
		Entry.LightColor = FLinearColor(0.2f, 0.22f, 0.35f, 1.0f);
		Entry.PeriodName = TEXT("Night");
		CausalitySchedule.Add(Entry);
	}
	{
		FDayNightCausalityEntry Entry;
		Entry.HourStart = 0.0f;
		Entry.HourEnd = 24.0f;
		Entry.ColorTempKelvin = 7000.0f;
		Entry.ShadowLengthMultiplier = 0.0f;
		Entry.ShadowSoftness = 1.0f;
		Entry.LightColor = FLinearColor(0.80f, 0.82f, 0.88f, 1.0f);
		Entry.PeriodName = TEXT("RainOvercast");
		CausalitySchedule.Add(Entry);
	}

	ApplyAutoQualityPreset();
	ApplyLumenColorBleed();
	ApplyColorBleedCausality();
	ApplyCellLightingConsistency();
	ApplyCausalitySchedule(DayNight.TimeOfDay, CurrentWeather);
	UpdateLightingFromZone(CurrentLightingZone);
}

void AGZGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateDayNightCycle(DeltaSeconds);
	UpdateWeatherTransition(DeltaSeconds);

	if (AdaptiveTransitionTimer > 0.0f)
	{
		AdaptiveTransitionTimer -= DeltaSeconds;
		float Alpha = 1.0f - (AdaptiveTransitionTimer / AdaptiveResolution.TransitionTime);
		CurrentAdaptiveScreenPercentage = FMath::Lerp(AdaptiveTransitionFrom, AdaptiveTransitionTarget, FMath::Clamp(Alpha, 0.0f, 1.0f));
	}
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
		QualitySettings.TSRFrameCount = 4;
		QualitySettings.NaniteQualityLevel = 2;
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Applied M1 quality: Screen%%=75, Shadow=1024, VolFog=96, SSR=2, Bloom=0.8, TSR=4f, Nanite=L2"));
		break;

	case EAppleSiliconChip::M2:
		QualitySettings.ScreenPercentage = 90.0f;
		QualitySettings.ShadowResolution = 1536;
		QualitySettings.VolumetricFogGridSize = 112;
		QualitySettings.SSRQuality = 3;
		QualitySettings.BloomIntensity = 0.9f;
		QualitySettings.TSRFrameCount = 6;
		QualitySettings.NaniteQualityLevel = 3;
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Applied M2 quality: Screen%%=90, Shadow=1536, VolFog=112, SSR=3, Bloom=0.9, TSR=6f, Nanite=L3"));
		break;

	case EAppleSiliconChip::M3:
		QualitySettings.ScreenPercentage = 100.0f;
		QualitySettings.ShadowResolution = 2048;
		QualitySettings.VolumetricFogGridSize = 128;
		QualitySettings.SSRQuality = 4;
		QualitySettings.BloomIntensity = 1.0f;
		QualitySettings.TSRFrameCount = 8;
		QualitySettings.NaniteQualityLevel = 4;
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Applied M3 quality: Screen%%=100, Shadow=2048, VolFog=128, SSR=4, Bloom=1.0, TSR=8f, Nanite=L4"));
		break;

	default:
		QualitySettings.ScreenPercentage = 70.0f;
		QualitySettings.ShadowResolution = 1024;
		QualitySettings.VolumetricFogGridSize = 80;
		QualitySettings.SSRQuality = 2;
		QualitySettings.BloomIntensity = 0.7f;
		QualitySettings.TSRFrameCount = 4;
		QualitySettings.NaniteQualityLevel = 2;
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Applied default quality (Unknown chip)"));
		break;
	}

	CurrentAdaptiveScreenPercentage = QualitySettings.ScreenPercentage;
}

void AGZGameMode::SetWeather(EGZWeatherType NewWeather)
{
	if (CurrentWeather == NewWeather && WeatherTransitionProgress >= 1.0f) return;

	TargetWeather = NewWeather;
	WeatherTransitionProgress = 0.0f;
	WeatherTransitionDuration = GetWeatherTransitionTime(CurrentWeather, NewWeather);
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

void AGZGameMode::UpdateAdaptiveResolution(float VehicleSpeedKmh, bool bIsInCBD, bool bIsInStreet, bool bIsFar)
{
	float Target = QualitySettings.ScreenPercentage;

	if (bIsInCBD)
	{
		Target = AdaptiveResolution.CBDScreenPercentage;
	}
	else if (bIsFar)
	{
		Target = AdaptiveResolution.FarScreenPercentage;
	}
	else if (bIsInStreet)
	{
		Target = AdaptiveResolution.StreetScreenPercentage;
	}

	if (VehicleSpeedKmh > AdaptiveResolution.VehicleHighSpeedThreshold)
	{
		Target = FMath::Min(Target, AdaptiveResolution.VehicleHighSpeedScreenPercentage);
	}

	if (FMath::Abs(Target - CurrentAdaptiveScreenPercentage) > 0.5f)
	{
		AdaptiveTransitionFrom = CurrentAdaptiveScreenPercentage;
		AdaptiveTransitionTarget = Target;
		AdaptiveTransitionTimer = AdaptiveResolution.TransitionTime;
	}
}

FLinearColor AGZGameMode::KelvinToRGB(float Kelvin) const
{
	float Temp = Kelvin / 100.0f;
	float R, G, B;

	if (Temp <= 66.0f)
	{
		R = 255.0f;
		G = FMath::Clamp(99.4708025861f * FMath::Loge(Temp) - 161.1195681661f, 0.0f, 255.0f);
		if (Temp <= 19.0f)
		{
			B = 0.0f;
		}
		else
		{
			B = FMath::Clamp(138.5177312231f * FMath::Loge(Temp - 10.0f) - 305.0447927307f, 0.0f, 255.0f);
		}
	}
	else
	{
		R = FMath::Clamp(329.698727446f * FMath::Pow(Temp - 60.0f, -0.1332047592f), 0.0f, 255.0f);
		G = FMath::Clamp(288.1221695283f * FMath::Pow(Temp - 60.0f, -0.0755148492f), 0.0f, 255.0f);
		B = 255.0f;
	}

	return FLinearColor(R / 255.0f, G / 255.0f, B / 255.0f, 1.0f);
}

bool AGZGameMode::IsHourInRange(float Hour, float RangeStart, float RangeEnd) const
{
	if (RangeStart <= RangeEnd)
	{
		return Hour >= RangeStart && Hour < RangeEnd;
	}
	else
	{
		return Hour >= RangeStart || Hour < RangeEnd;
	}
}

void AGZGameMode::UpdateDayNightCycle(float DeltaSeconds)
{
	DayNight.TimeOfDay += DeltaSeconds * (24.0f / DayNight.DayLengthSeconds);
	if (DayNight.TimeOfDay >= 24.0f) DayNight.TimeOfDay -= 24.0f;

	const float Hour = DayNight.TimeOfDay;
	ApplyCausalitySchedule(Hour, CurrentWeather);
}

void AGZGameMode::ApplyCausalitySchedule(float Hour, EGZWeatherType Weather)
{
	if (Weather == EGZWeatherType::FogHaze)
	{
		if (SkyComponent)
		{
			SkyComponent->SetIntensity(0.3f);
		}
		if (SunComponent)
		{
			SunComponent->SetIntensity(0.0f);
		}

		AExponentialHeightFog* FogActor = Cast<AExponentialHeightFog>(UGameplayStatics::GetActorOfClass(GetWorld(), AExponentialHeightFog::StaticClass()));
		if (FogActor)
		{
			UExponentialHeightFogComponent* FogComp = FogActor->GetComponent();
			if (FogComp)
			{
				FogComp->SetFogMaxOpacity(1.0f);
				FogComp->SetFogCutoffDistance(8000.0f);
				FogComp->SetStartDistance(0.0f);
			}
		}

		FLinearColor FogColor = KelvinToRGB(7000.0f);
		if (SkyComponent)
		{
			SkyComponent->SetLightColor(FogColor);
		}

		UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("Causality: Fog - 7000K, visibility 80m, ambient only"));
		return;
	}

	if (Weather == EGZWeatherType::Rain || Weather == EGZWeatherType::Storm)
	{
		if (SunComponent)
		{
			SunComponent->SetIntensity(0.0f);
		}
		if (SkyComponent)
		{
			SkyComponent->SetIntensity(0.3f);
		}

		FLinearColor RainColor = KelvinToRGB(7000.0f);
		if (SkyComponent)
		{
			SkyComponent->SetLightColor(RainColor);
		}

		UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("Causality: Rain/Overcast - 7000K, no direct sun, contrast -40%%, ambient only"));
		return;
	}

	const FDayNightCausalityEntry* ActiveEntry = nullptr;
	for (const FDayNightCausalityEntry& Entry : CausalitySchedule)
	{
		if (IsHourInRange(Hour, Entry.HourStart, Entry.HourEnd))
		{
			ActiveEntry = &Entry;
			break;
		}
	}

	if (!ActiveEntry)
	{
		for (const FDayNightCausalityEntry& Entry : CausalitySchedule)
		{
			if (Entry.PeriodName == TEXT("Night"))
			{
				ActiveEntry = &Entry;
				break;
			}
		}
	}

	if (!ActiveEntry) return;

	FLinearColor KelvinColor = KelvinToRGB(ActiveEntry->ColorTempKelvin);

	if (SunComponent)
	{
		if (ActiveEntry->PeriodName == TEXT("Night"))
		{
			SunComponent->SetIntensity(0.0f);
		}
		else
		{
			SunComponent->SetIntensity(DayNight.SunIntensity);
		}
		SunComponent->SetLightColor(KelvinColor);

		SunComponent->SetShadowBias(ActiveEntry->ShadowSoftness);

		float BaseSunAngle = (Hour - 6.0f) * 15.0f;
		float SunAngleDeg = BaseSunAngle;
		if (ActiveEntry->ShadowLengthMultiplier > 0.0f)
		{
			SunAngleDeg = FMath::Lerp(90.0f, -20.0f, FMath::Clamp(ActiveEntry->ShadowLengthMultiplier / 5.0f, 0.0f, 1.0f));
		}
		DayNight.SunAngle = SunAngleDeg;

		FRotator SunRot(-SunAngleDeg + 90.0f, 0.0f, 0.0f);
		if (SunLight)
		{
			SunLight->SetActorRotation(SunRot);
		}
	}

	if (SkyComponent)
	{
		if (ActiveEntry->PeriodName == TEXT("Night"))
		{
			SkyComponent->SetIntensity(0.5f);
			SkyComponent->SetLightColor(KelvinColor);
		}
		else
		{
			SkyComponent->SetIntensity(2.0f);
			SkyComponent->SetLightColor(KelvinColor);
		}
	}

	UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("Causality: %s - %.0fK, shadow=%.1fx, softness=%.2f"),
		*ActiveEntry->PeriodName, ActiveEntry->ColorTempKelvin,
		ActiveEntry->ShadowLengthMultiplier, ActiveEntry->ShadowSoftness);
}

void AGZGameMode::ApplyColorBleedCausality()
{
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.MaxDistance"))->Set(ColorBleedCausality.MaxBleedDistance);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.DecayMode"))->Set(1);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.WallIntensity"))->Set(ColorBleedCausality.WallBleed);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.GlassIntensity"))->Set(ColorBleedCausality.GlassBleed);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.RoadIntensity"))->Set(ColorBleedCausality.RoadBleed);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.VegetationIntensity"))->Set(ColorBleedCausality.VegetationBleed);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.RedWallIntensity"))->Set(ColorBleedCausality.RedWallBleed);

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("ColorBleedCausality: maxDist=%.0f, wall=%.2f, glass=%.2f, road=%.2f, veg=%.2f, redwall=%.2f"),
		ColorBleedCausality.MaxBleedDistance, ColorBleedCausality.WallBleed,
		ColorBleedCausality.GlassBleed, ColorBleedCausality.RoadBleed,
		ColorBleedCausality.VegetationBleed, ColorBleedCausality.RedWallBleed);
}

void AGZGameMode::ApplyCellLightingConsistency()
{
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.WorldPartition.LightingConsistency.MaxBrightnessDelta"))->Set(CellLightingConsistency.MaxBrightnessDelta);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.WorldPartition.LightingConsistency.MaxColorTempDelta"))->Set(CellLightingConsistency.MaxColorTempDelta);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.WorldPartition.LightingConsistency.TransitionCells"))->Set(CellLightingConsistency.TransitionZoneCells);

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("CellLightingConsistency: brightnessDelta=%.4f, tempDelta=%.0fK, transitionCells=%d"),
		CellLightingConsistency.MaxBrightnessDelta, CellLightingConsistency.MaxColorTempDelta,
		CellLightingConsistency.TransitionZoneCells);
}

void AGZGameMode::UpdateWeatherTransition(float DeltaSeconds)
{
	if (WeatherTransitionProgress >= 1.0f) return;

	WeatherTransitionProgress += DeltaSeconds / WeatherTransitionDuration;
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

	const int32 SampleCount = GetLightingZoneSampleCount(Zone);

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

	UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("Lighting zone: %d, samples=%d"), (int32)Zone, SampleCount);
}

void AGZGameMode::ApplyAutoQualityPreset()
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Auto quality: Screen%%=%.0f, Shadow=%d, VolFog=%d, SSR=%d, Bloom=%.1f, TSR=%df, Nanite=L%d"),
		QualitySettings.ScreenPercentage, QualitySettings.ShadowResolution,
		QualitySettings.VolumetricFogGridSize, QualitySettings.SSRQuality, QualitySettings.BloomIntensity,
		QualitySettings.TSRFrameCount, QualitySettings.NaniteQualityLevel);
}

void AGZGameMode::ApplyLumenColorBleed()
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Lumen color bleed: wall=%.2f, glass=%.2f, road=%.2f"),
		LumenColorBleed.WallBleed, LumenColorBleed.GlassBleed, LumenColorBleed.RoadBleed);
}

float AGZGameMode::GetWeatherTransitionTime(EGZWeatherType From, EGZWeatherType To) const
{
	if (From == To) return 0.0f;

	if (From == EGZWeatherType::Clear && To == EGZWeatherType::Cloudy) return 2.8f;
	if (From == EGZWeatherType::Cloudy && To == EGZWeatherType::Clear) return 2.8f;
	if (From == EGZWeatherType::Cloudy && To == EGZWeatherType::Rain) return 4.1f;
	if (From == EGZWeatherType::Rain && To == EGZWeatherType::Cloudy) return 4.1f;
	if (From == EGZWeatherType::Rain && To == EGZWeatherType::FogHaze) return 6.5f;
	if (From == EGZWeatherType::FogHaze && To == EGZWeatherType::Rain) return 6.5f;

	return 5.0f;
}

int32 AGZGameMode::GetLightingZoneSampleCount(EGZLightingZone Zone) const
{
	switch (Zone)
	{
	case EGZLightingZone::OutdoorStreet: return 2048;
	case EGZLightingZone::Indoor:        return 1024;
	case EGZLightingZone::Tunnel:        return 512;
	case EGZLightingZone::RiverSurface:  return 2048;
	default: return 2048;
	}
}
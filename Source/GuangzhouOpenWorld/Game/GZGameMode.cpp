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

	// Initialize v5.0 default parameters per requirements
	BleedDecay.NearStart = 0.0f;
	BleedDecay.NearEnd = 200.0f;
	BleedDecay.NearDecay = 0.60f;
	BleedDecay.MidStart = 200.0f;
	BleedDecay.MidEnd = 500.0f;
	BleedDecay.MidDecay = 0.30f;
	BleedDecay.FarStart = 500.0f;
	BleedDecay.FarEnd = 800.0f;
	BleedDecay.FarDecay = 0.10f;

	VegetationBleed.BaseBleedIntensity = 0.05f;
	VegetationBleed.CanopyBleedBoost = 0.07f;
	VegetationBleed.ShrubBleedReduction = 0.04f;
	VegetationBleed.BroadleafRangeBoost = 1.10f;
	VegetationBleed.NeedleleafIntensityScale = 0.85f;

	WallBleed.BaseBleedIntensity = 0.22f;
	WallBleed.DirectSunBoost = 0.09f;
	WallBleed.SideLightReduction = 0.06f;
	WallBleed.ProjectionAngle = 30.0f;
	WallBleed.RoughBleedSpread = 0.08f;
	WallBleed.SmoothTileFocus = 0.0f;
	WallBleed.bOcclusionFromSurface = true;

	RoadBleed.ReceiveBleedIntensity = 0.15f;
	RoadBleed.EmitBleedIntensity = 0.0f;
	RoadBleed.BrightnessReduction = 0.03f;
	RoadBleed.WornRoadAbsorptionBoost = 0.05f;
	RoadBleed.NewRoadContrastBoost = 0.02f;

	CellBlend.BlendWidthCells = 2;
	CellBlend.BlendWidthMeters = 256.0f;
	CellBlend.bUseCosineCurve = true;
	CellBlend.ColorTempBlendThreshold = 300.0f;
	CellBlend.bBlendShadowHardness = true;
	CellBlend.bBlendShadowDirection = true;
	CellBlend.bBlendBleedIntensity = true;
	CellBlend.bBlendAmbientOcclusion = true;

	WeatherLayers.TopAmbientBrightness = 0.12f;
	WeatherLayers.TopAmbientColorTemp = 6800.0f;
	WeatherLayers.SideAmbientBrightness = 0.07f;
	WeatherLayers.SideAmbientColorTemp = 7200.0f;
	WeatherLayers.EdgeContrastRetention = 0.70f;
	WeatherLayers.VisibilityNearNoDecay = 4000.0f;
	WeatherLayers.VisibilityFarDecay = 8000.0f;
	WeatherLayers.FarBrightnessFloor = 0.85f;

	FogLighting.SatNearNoDecay = 2000.0f;
	FogLighting.SatMidDecay = 5000.0f;
	FogLighting.SatFarDecay = 8000.0f;
	FogLighting.ShadowBlurMultiplier = 2.0f;
	FogLighting.ShadowLengthReduction = 0.15f;
	FogLighting.LowElevationFogBoost = 0.15f;
	FogLighting.CoolColorDecayFaster = 1.3f;
	FogLighting.WarmColorRetention = 0.8f;

	WeatherPhases.Phase1_DirectSunReduction = 1.2f;
	WeatherPhases.Phase2_ColorTempShift = 0.8f;
	WeatherPhases.Phase3_ContrastShadow = 0.8f;
	WeatherPhases.CloudPreAnimation = 1.0f;
	WeatherPhases.bStormRapidDarkening = true;

	RoadWetnessParams.WetTransitionTime = 30.0f;
	RoadWetnessParams.DryTimeClear = 480.0f;
	RoadWetnessParams.DryTimeCloudy = 900.0f;
	RoadWetnessParams.WetReflectivityBoost = 0.22f;
	RoadWetnessParams.WetRoughnessReduction = 0.16f;
	RoadWetnessParams.PuddleReflectivityExtra = 0.20f;
	RoadWetnessParams.PuddleEvapTimeScale = 0.70f;
	RoadWetnessParams.SunFacingDrySpeedBoost = 1.40f;
	RoadWetnessParams.ShadeFacingDrySpeedReduction = 0.80f;

	WindParams.BranchSwingPeriodMin = 8.0f;
	WindParams.BranchSwingPeriodMax = 12.0f;
	WindParams.LeafSwingPeriodMin = 2.0f;
	WindParams.LeafSwingPeriodMax = 4.0f;
	WindParams.WindGustAmplitude = 0.3f;
	WindParams.HeavyMetalSwingScale = 0.3f;
	WindParams.LightClothSwingScale = 2.0f;
	WindParams.ThickBranchSwingScale = 0.2f;
	WindParams.ThinBranchSwingScale = 1.5f;
	WindParams.LocalWindVariation = 0.15f;
	WindParams.SignSwingAngleRange = 15.0f;
	WindParams.SignSwingPeriodMin = 3.0f;
	WindParams.SignSwingPeriodMax = 7.0f;

	// TSR distance weights - 7 tiers
	TSRDistanceWeights.Empty();
	{
		FTSRDistanceWeight Tier0; Tier0.Tier = EGZTSRDistanceTier::Near_0_50; Tier0.MinDistance = 0.0f; Tier0.MaxDistance = 5000.0f; Tier0.HistoryWeightBoost = 0.00f; Tier0.FrameCacheCount = 6;
		FTSRDistanceWeight Tier1; Tier1.Tier = EGZTSRDistanceTier::MidNear_50_100; Tier1.MinDistance = 5000.0f; Tier1.MaxDistance = 10000.0f; Tier1.HistoryWeightBoost = 0.05f; Tier1.FrameCacheCount = 6;
		FTSRDistanceWeight Tier2; Tier2.Tier = EGZTSRDistanceTier::Mid_100_200; Tier2.MinDistance = 10000.0f; Tier2.MaxDistance = 20000.0f; Tier2.HistoryWeightBoost = 0.08f; Tier2.FrameCacheCount = 7;
		FTSRDistanceWeight Tier3; Tier3.Tier = EGZTSRDistanceTier::MidFar_200_400; Tier3.MinDistance = 20000.0f; Tier3.MaxDistance = 40000.0f; Tier3.HistoryWeightBoost = 0.12f; Tier3.FrameCacheCount = 7;
		FTSRDistanceWeight Tier4; Tier4.Tier = EGZTSRDistanceTier::Far_400_800; Tier4.MinDistance = 40000.0f; Tier4.MaxDistance = 80000.0f; Tier4.HistoryWeightBoost = 0.15f; Tier4.FrameCacheCount = 8;
		FTSRDistanceWeight Tier5; Tier5.Tier = EGZTSRDistanceTier::VeryFar_800_1500; Tier5.MinDistance = 80000.0f; Tier5.MaxDistance = 150000.0f; Tier5.HistoryWeightBoost = 0.18f; Tier5.FrameCacheCount = 8;
		FTSRDistanceWeight Tier6; Tier6.Tier = EGZTSRDistanceTier::Skyline_1500Plus; Tier6.MinDistance = 150000.0f; Tier6.MaxDistance = 1000000.0f; Tier6.HistoryWeightBoost = 0.20f; Tier6.FrameCacheCount = 8;
		TSRDistanceWeights.Add(Tier0); TSRDistanceWeights.Add(Tier1); TSRDistanceWeights.Add(Tier2); TSRDistanceWeights.Add(Tier3);
		TSRDistanceWeights.Add(Tier4); TSRDistanceWeights.Add(Tier5); TSRDistanceWeights.Add(Tier6);
	}

	TSRSharpening.BaseSharpening = 0.18f;
	TSRSharpening.LinearTextureBoost = 0.08f;
	TSRSharpening.LinearTextureMinPx = 1;
	TSRSharpening.LinearTextureMaxPx = 3;
	TSRSharpening.TrafficSignSharpening = 0.12f;
	TSRSharpening.BuildingTextSharpening = 0.08f;
	TSRSharpening.RoadTextureSharpening = 0.04f;
	TSRSharpening.BrightLightReduction = 0.7f;
	TSRSharpening.LowLightBoost = 1.2f;

	GlassLayerParams.Layer1IOR = 1.52f;
	GlassLayerParams.Layer1BrightnessDecay = 0.18f;
	GlassLayerParams.Layer2IOR = 1.52f;
	GlassLayerParams.Layer2BrightnessDecay = 0.12f;
	GlassLayerParams.Layer2RefractionReduction = 0.50f;
	GlassLayerParams.AirGapDecay = 0.03f;
	GlassLayerParams.TotalContrastReduction = 0.25f;
	GlassLayerParams.FrameSmoothPixels = 0.5f;
	GlassLayerParams.MaxLayers = 3;

	FresnelParams.View0Reflect = 0.05f;
	FresnelParams.View60Reflect = 0.15f;
	FresnelParams.View85Reflect = 0.55f;
	FresnelParams.View90Reflect = 0.74f;
	FresnelParams.SunHighlightShrink = 0.20f;
	FresnelParams.SunHighlightBoost = 0.15f;
	FresnelParams.bRoughnessBindsFresnel = true;

	GlassNeutrality.bRemoveBlueFilter = true;
	GlassNeutrality.ThickBottomThreshold = 0.35f;
	GlassNeutrality.MaxColorShiftKelvin = 50.0f;
	GlassNeutrality.DuskColorShift = 30.0f;
	GlassNeutrality.EdgeThicknessColorFade = 0.05f;

	// Initialize state
	RoadWetness = 0.0f;
	RoadWetnessTarget = 0.0f;
	RoadWetState = EGZRoadWetState::Dry;
	WindStrength = 0.0f;

	// v5.1 Cloud Tiers - 3 entries (15-45s random flow, ±0.08 shadow variation)
	CloudTiers.Empty();
	{
		FCloudTierLighting Heavy; Heavy.TopAmbientReduction = -0.06f; Heavy.ShadowVariationPeriod = 4.0f; Heavy.ShadowVariationAmplitude = 0.08f; Heavy.CloudFlowPeriodMin = 15.0f; Heavy.CloudFlowPeriodMax = 45.0f;
		FCloudTierLighting Medium; Medium.TopAmbientReduction = 0.0f; Medium.ShadowVariationPeriod = 4.0f; Medium.ShadowVariationAmplitude = 0.08f; Medium.CloudFlowPeriodMin = 15.0f; Medium.CloudFlowPeriodMax = 45.0f;
		FCloudTierLighting Thin; Thin.TopAmbientReduction = 0.05f; Thin.ShadowVariationPeriod = 4.0f; Thin.ShadowVariationAmplitude = 0.08f; Thin.CloudFlowPeriodMin = 15.0f; Thin.CloudFlowPeriodMax = 45.0f;
		CloudTiers.Add(Heavy); CloudTiers.Add(Medium); CloudTiers.Add(Thin);
	}

	// v5.1 RayTracingConfig (distance-tiered samples: 0-50m=32, 50-200m=16, 200m+=8)
	RayTracingConfig.bEnableRTGI = true;
	RayTracingConfig.bEnableRTShadows = true;
	RayTracingConfig.bEnableRTAO = true;
	RayTracingConfig.bEnableRTReflections = true;
	RayTracingConfig.bEnableDLSS = true;
	RayTracingConfig.bEnableFSR = true;
	RayTracingConfig.bEnableFrameGen = true;
	RayTracingConfig.RTSamplesPerPixel = 1;
	RayTracingConfig.RTNearSamples = 32;
	RayTracingConfig.RTMidSamples = 16;
	RayTracingConfig.RTFarSamples = 8;
	RayTracingConfig.RTNearDistance = 5000.0f;
	RayTracingConfig.RTMidDistance = 20000.0f;
	RayTracingConfig.bEnableContactShadows = true;
	RayTracingConfig.ContactShadowSamples = 16;
	RayTracingConfig.RTShadowNearSamples = 32;
	RayTracingConfig.RTShadowMidSamples = 16;
	RayTracingConfig.RTShadowFarSamples = 8;
	RayTracingConfig.bEnablePathTracingReflections = true;
	RayTracingConfig.NeonLightSamples = 64;
	RayTracingConfig.ReflectionNearSamples = 32;
	RayTracingConfig.ReflectionMidSamples = 16;
	RayTracingConfig.ReflectionFarSamples = 8;

	// v5.1 DirectStorageConfig
	DirectStorageConfig.bEnableDirectStorage = true;
	DirectStorageConfig.PreloadPriority1_Distance = 4.0f;
	DirectStorageConfig.PreloadPriority2_Distance = 3.0f;
	DirectStorageConfig.PreloadPriority3_Distance = 2.0f;

	// v5.1 NPCLifecycle
	NPCLifecycle.bEnableMemory = true;
	NPCLifecycle.MemoryRetentionHours = 24.0f;
	NPCLifecycle.bEnableInterNPCInteraction = true;
	NPCLifecycle.InteractionRadius = 3000.0f;
	NPCLifecycle.InteractionFrequencyMin = 10.0f;
	NPCLifecycle.InteractionFrequencyMax = 60.0f;

	// v5.1 TrafficBehaviors - 5 entries
	TrafficBehaviors.Empty();
	{
		FTrafficAIExpanded Normal; Normal.DecelerationBuffer = 0.0f; Normal.SmoothTurnRadius = 0.0f; Normal.RainBrakingExtension = 0.0f; Normal.RainTurnRadiusIncrease = 0.0f; Normal.WaterSplashThreshold = 0.0f; Normal.WaterSplashMaxHeight = 0.0f; Normal.bEnableAutoLights = false; Normal.TunnelLightTrigger = 0.0f; Normal.NightLightTrigger = 0.0f;
		FTrafficAIExpanded RushHour; RushHour.DecelerationBuffer = 0.5f; RushHour.SmoothTurnRadius = 5.0f; RushHour.RainBrakingExtension = 0.0f; RushHour.RainTurnRadiusIncrease = 0.0f; RushHour.WaterSplashThreshold = 0.0f; RushHour.WaterSplashMaxHeight = 0.0f; RushHour.bEnableAutoLights = false; RushHour.TunnelLightTrigger = 0.0f; RushHour.NightLightTrigger = 0.0f;
		FTrafficAIExpanded NightSparse; NightSparse.DecelerationBuffer = 0.0f; NightSparse.SmoothTurnRadius = 0.0f; NightSparse.RainBrakingExtension = 0.0f; NightSparse.RainTurnRadiusIncrease = 0.0f; NightSparse.WaterSplashThreshold = 0.0f; NightSparse.WaterSplashMaxHeight = 0.0f; NightSparse.bEnableAutoLights = true; NightSparse.TunnelLightTrigger = 0.0f; NightSparse.NightLightTrigger = 18.0f;
		FTrafficAIExpanded RainCautious; RainCautious.DecelerationBuffer = 1.0f; RainCautious.SmoothTurnRadius = 0.0f; RainCautious.RainBrakingExtension = 15.0f; RainCautious.RainTurnRadiusIncrease = 10.0f; RainCautious.WaterSplashThreshold = 0.3f; RainCautious.WaterSplashMaxHeight = 150.0f; RainCautious.bEnableAutoLights = true; RainCautious.TunnelLightTrigger = 0.0f; RainCautious.NightLightTrigger = 0.0f;
		FTrafficAIExpanded FogSlow; FogSlow.DecelerationBuffer = 2.0f; FogSlow.SmoothTurnRadius = 10.0f; FogSlow.RainBrakingExtension = 0.0f; FogSlow.RainTurnRadiusIncrease = 0.0f; FogSlow.WaterSplashThreshold = 0.0f; FogSlow.WaterSplashMaxHeight = 0.0f; FogSlow.bEnableAutoLights = true; FogSlow.TunnelLightTrigger = 1.0f; FogSlow.NightLightTrigger = 0.0f;
		TrafficBehaviors.Add(Normal); TrafficBehaviors.Add(RushHour); TrafficBehaviors.Add(NightSparse); TrafficBehaviors.Add(RainCautious); TrafficBehaviors.Add(FogSlow);
	}

	// v5.1 FogSpatialization (low elevation +30%, high elevation -20%, cool decay 20% faster, warm retain +15%)
	FogSpatialization.LowElevationBoost = 0.30f;
	FogSpatialization.HighElevationReduction = 0.20f;
	FogSpatialization.CoolWavelengthDecay = 1.2f;
	FogSpatialization.WarmWavelengthRetention = 0.85f;
	FogSpatialization.ElevationThreshold = 5000.0f;

	// v5.1 Weather Pre-Transition: brightness drops 0.1 one second before storm
	WeatherPreTransition.PreTransitionTime = 1.0f;
	WeatherPreTransition.StormDarkeningIntensity = 0.1f;
	WeatherPreTransition.TransitionCurveType = TEXT("easeInOut");

	// v5.1 NaniteSeamFix
	NaniteSeamFix.VertexInterpolationDistance = 0.01f;
	NaniteSeamFix.MaxSeamGap = 0.01f;
	NaniteSeamFix.bEnableSeamBlend = true;
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
	ApplyCellBlendRegion();
	ApplyVegetationBleedParams(0.0f);
	ApplyWallBleedParams(0.0f, 45.0f);
	ApplyRoadBleedParams();
	ApplyGlassRefractionParams();
	ApplyRefinedFresnelParams();
	ApplyCausalitySchedule(DayNight.TimeOfDay, CurrentWeather);
	ApplyWeatherLayeredLighting(DayNight.TimeOfDay, CurrentWeather);
	UpdateLightingFromZone(CurrentLightingZone);

	// v5.1
	ApplyRayTracingConfig();
	ApplyDirectStorageConfig();
	ApplyNaniteSeamFix();
}

void AGZGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateDayNightCycle(DeltaSeconds);
	UpdateWeatherTransition(DeltaSeconds);
	UpdateRoadWetness(DeltaSeconds);
	UpdateVegetationWind(DeltaSeconds);

	if (AdaptiveTransitionTimer > 0.0f)
	{
		AdaptiveTransitionTimer -= DeltaSeconds;
		float Alpha = 1.0f - (AdaptiveTransitionTimer / AdaptiveResolution.TransitionTime);
		CurrentAdaptiveScreenPercentage = FMath::Lerp(AdaptiveTransitionFrom, AdaptiveTransitionTarget, FMath::Clamp(Alpha, 0.0f, 1.0f));
	}

	// v5.1 Fog spatialization, TSR distance weights, and weather pre-transition
	float Elevation = 0.0f;
	float CameraDistance = 0.0f;
	float VehicleSpeedKmh = 0.0f;
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (APawn* Pawn = PC->GetPawn())
		{
			Elevation = Pawn->GetActorLocation().Z;
			CameraDistance = Pawn->GetVelocity().Size();
			VehicleSpeedKmh = CameraDistance * 0.036f; // cm/s to km/h
		}
	}
	ApplyFogSpatialization(Elevation);
	ApplyTSRDistanceWeights(CameraDistance, VehicleSpeedKmh);

	if (WeatherTransitionProgress < 1.0f)
	{
		ApplyWeatherPreTransition(DeltaSeconds);
	}
}

void AGZGameMode::SetRoadWetnessTarget(float Target)
{
	RoadWetnessTarget = FMath::Clamp(Target, 0.0f, 1.0f);
	if (RoadWetnessTarget > 0.01f && RoadWetState == EGZRoadWetState::Dry)
	{
		RoadWetState = EGZRoadWetState::Transitioning;
	}
}

void AGZGameMode::SetWindStrength(float Strength)
{
	WindStrength = FMath::Max(0.0f, Strength);
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

	if (AdaptiveResolution.bUseEightSteps)
	{
		// 8 smooth resolution steps: 100, 95, 90, 85, 80, 75, 70, 65
		if (bIsInCBD) Target = AdaptiveResolution.Step0_Ultra;
		else if (bIsInStreet) Target = AdaptiveResolution.Step2_MediumHigh;
		else if (bIsFar) Target = AdaptiveResolution.Step5_Low;
		else Target = AdaptiveResolution.Step1_High;

		if (VehicleSpeedKmh > AdaptiveResolution.VehicleHighSpeedThreshold)
		{
			Target = FMath::Min(Target, AdaptiveResolution.VehicleHighSpeedScreenPercentage);
		}
	}
	else
	{
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
	if (FogActor)
	{
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

	// 2.8s three-stage weather transition: direct sun -> color temp -> contrast/shadows
	const float T = WeatherTransitionProgress * WeatherTransitionDuration;
	float SunAlpha = 0.0f, TempAlpha = 0.0f, ContrastAlpha = 0.0f;

	if (T <= WeatherStages.DirectSunEnd)
	{
		SunAlpha = T / WeatherStages.DirectSunEnd;
	}
	else if (T <= WeatherStages.ColorTempEnd)
	{
		SunAlpha = 1.0f;
		TempAlpha = (T - WeatherStages.DirectSunEnd) / (WeatherStages.ColorTempEnd - WeatherStages.DirectSunEnd);
	}
	else if (T <= WeatherStages.FinalEnd)
	{
		SunAlpha = 1.0f;
		TempAlpha = 1.0f;
		ContrastAlpha = (T - WeatherStages.ColorTempEnd) / (WeatherStages.FinalEnd - WeatherStages.ColorTempEnd);
	}
	else
	{
		SunAlpha = TempAlpha = ContrastAlpha = 1.0f;
	}

	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Weather.Transition.DirectSunAlpha"))->Set(SunAlpha);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Weather.Transition.ColorTempAlpha"))->Set(TempAlpha);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Weather.Transition.ContrastAlpha"))->Set(ContrastAlpha);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Weather.Transition.RainColorTemp"))->Set(WeatherStages.RainColorTempK);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Weather.Transition.ContrastDrop"))->Set(WeatherStages.ContrastDrop);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Weather.Transition.ShadowBlurBoost"))->Set(WeatherStages.ShadowBlurBoost);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Weather.Transition.ShadowLengthScale"))->Set(WeatherStages.ShadowLengthScale);
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

// ============================================================================
// v5.0 Math Utilities
// ============================================================================
float AGZGameMode::CosineBlendWeight(float T) const
{
	float Clamped = FMath::Clamp(T, 0.0f, 1.0f);
	return 0.5f - 0.5f * FMath::Cos(Clamped * PI);
}

float AGZGameMode::QuarticBlendWeight(float T) const
{
	float Clamped = FMath::Clamp(T, 0.0f, 1.0f);
	float t2 = Clamped * Clamped;
	return t2 * t2 * (35.0f - 84.0f * Clamped + 70.0f * t2 - 20.0f * t2 * Clamped);
}

float AGZGameMode::QuarticCosineBlendWeight(float T) const
{
	float Clamped = FMath::Clamp(T, 0.0f, 1.0f);
	// UE5.8 quartic-cosine smoothing: cosine easing first, then quartic falloff
	float CosineT = 0.5f - 0.5f * FMath::Cos(Clamped * PI);
	float t2 = CosineT * CosineT;
	return t2 * t2 * (35.0f - 84.0f * CosineT + 70.0f * t2 - 20.0f * t2 * CosineT);
}

// ============================================================================
// v5.0 Cell Blend Region - 2-cell width, quartic curve, full parameter blend
// ============================================================================
void AGZGameMode::ApplyCellBlendRegion()
{
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.WorldPartition.CellBlend.WidthCells"))->Set(CellBlend.BlendWidthCells);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.WorldPartition.CellBlend.WidthMeters"))->Set(CellBlend.BlendWidthMeters);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.WorldPartition.CellBlend.CosineCurve"))->Set(CellBlend.bUseCosineCurve ? 1 : 0);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.WorldPartition.CellBlend.BlendCurve"))->Set((int32)CellBlend.BlendCurve);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.WorldPartition.CellBlend.ColorTempThreshold"))->Set(CellBlend.ColorTempBlendThreshold);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.WorldPartition.CellBlend.ShadowHardness"))->Set(CellBlend.bBlendShadowHardness ? 1 : 0);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.WorldPartition.CellBlend.ShadowDirection"))->Set(CellBlend.bBlendShadowDirection ? 1 : 0);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.WorldPartition.CellBlend.BleedIntensity"))->Set(CellBlend.bBlendBleedIntensity ? 1 : 0);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.WorldPartition.CellBlend.AmbientOcclusion"))->Set(CellBlend.bBlendAmbientOcclusion ? 1 : 0);

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("CellBlend: width=%d cells, curve=%d, tempBlend=%.0fK, shadow=%d, bleed=%d, ao=%d"),
		CellBlend.BlendWidthCells, (int32)CellBlend.BlendCurve, CellBlend.ColorTempBlendThreshold,
		CellBlend.bBlendShadowHardness, CellBlend.bBlendBleedIntensity, CellBlend.bBlendAmbientOcclusion);
}

// ============================================================================
// v5.0 Weather Layered Lighting - 2-layer ambient for rain, fog saturation decay
// ============================================================================
void AGZGameMode::ApplyWeatherLayeredLighting(float Hour, EGZWeatherType Weather)
{
	if (Weather == EGZWeatherType::Rain || Weather == EGZWeatherType::Storm)
	{
		IConsoleManager::Get().FindConsoleVariable(TEXT("r.AmbientLighting.TopLayerBrightness"))->Set(WeatherLayers.TopAmbientBrightness);
		IConsoleManager::Get().FindConsoleVariable(TEXT("r.AmbientLighting.TopLayerColorTemp"))->Set(WeatherLayers.TopAmbientColorTemp);
		IConsoleManager::Get().FindConsoleVariable(TEXT("r.AmbientLighting.SideLayerBrightness"))->Set(WeatherLayers.SideAmbientBrightness);
		IConsoleManager::Get().FindConsoleVariable(TEXT("r.AmbientLighting.SideLayerColorTemp"))->Set(WeatherLayers.SideAmbientColorTemp);
		IConsoleManager::Get().FindConsoleVariable(TEXT("r.AmbientLighting.EdgeContrastRetention"))->Set(WeatherLayers.EdgeContrastRetention);

		// Cloud thickness modulation
		float CloudThickness = (Weather == EGZWeatherType::Storm) ? 0.18f : 0.08f;
		float TopAdj = FMath::Lerp(WeatherLayers.TopAmbientBrightness, WeatherLayers.TopAmbientBrightness - 0.06f, CloudThickness);
		IConsoleManager::Get().FindConsoleVariable(TEXT("r.AmbientLighting.TopLayerBrightness"))->Set(TopAdj);

		UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("WeatherLayers: rain top=%.3f(%.0fK) side=%.3f(%.0fK) edge=%.2f cloud=%.2f"),
			WeatherLayers.TopAmbientBrightness, WeatherLayers.TopAmbientColorTemp,
			WeatherLayers.SideAmbientBrightness, WeatherLayers.SideAmbientColorTemp,
			WeatherLayers.EdgeContrastRetention, CloudThickness);
	}

	if (Weather == EGZWeatherType::FogHaze)
	{
		ApplyFogLightingParams(Hour);
	}
}

void AGZGameMode::ApplyFogLightingParams(float Hour)
{
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Fog.SaturationNearNoDecay"))->Set(FogLighting.SatNearNoDecay);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Fog.SaturationMidDecay"))->Set(FogLighting.SatMidDecay);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Fog.SaturationFarDecay"))->Set(FogLighting.SatMidDecay);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Fog.FarBrightnessFloor"))->Set(FogLighting.FarBrightnessFloor);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Fog.ShadowBlurMultiplier"))->Set(FogLighting.ShadowBlurMultiplier);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Fog.ShadowLengthReduction"))->Set(FogLighting.ShadowLengthReduction);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Fog.LowElevationBoost"))->Set(FogLighting.LowElevationFogBoost);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Fog.HighElevationReduction"))->Set(FogSpatialization.HighElevationReduction);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Fog.CoolColorDecayFaster"))->Set(FogLighting.CoolColorDecayFaster);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Fog.WarmColorRetention"))->Set(FogLighting.WarmColorRetention);

	UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("FogParams: satRange=%.0f/%.0f/%.0f shadowBlur=%.1fx shadowLen=-%.0f%%"),
		FogLighting.SatNearNoDecay, FogLighting.SatMidDecay, FogLighting.SatFarDecay,
		FogLighting.ShadowBlurMultiplier, FogLighting.ShadowLengthReduction * 100.0f);
}

// ============================================================================
// v5.0 Vegetation Bleed - per-type, gradient decay, sun-angle offset
// ============================================================================
void AGZGameMode::ApplyVegetationBleedParams(float SunAzimuth)
{
	// Broadleaf: range +10%, needleleaf: intensity -15%, tall tree canopy: +0.07, shrub: -0.04
	float BroadleafIntensity = VegetationBleed.BaseBleedIntensity * VegetationBleed.BroadleafRangeBoost;
	float NeedleleafIntensity = VegetationBleed.BaseBleedIntensity * VegetationBleed.NeedleleafIntensityScale;
	float ShrubIntensity = FMath::Max(0.0f, VegetationBleed.BaseBleedIntensity - VegetationBleed.ShrubBleedReduction);
	float TallTreeIntensity = VegetationBleed.BaseBleedIntensity + VegetationBleed.CanopyBleedBoost;

	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.BroadleafIntensity"))->Set(BroadleafIntensity);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.NeedleleafIntensity"))->Set(NeedleleafIntensity);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.ShrubIntensity"))->Set(ShrubIntensity);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.TallTreeIntensity"))->Set(TallTreeIntensity);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.SunAzimuthOffset"))->Set(SunAzimuth);

	// Gradient decay: 0-2m=60%, 2-5m=30%, 5-8m=10%
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.DecayNearStart"))->Set(BleedDecay.NearStart);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.DecayNearEnd"))->Set(BleedDecay.NearEnd);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.DecayNearRate"))->Set(BleedDecay.NearDecay);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.DecayMidStart"))->Set(BleedDecay.MidStart);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.DecayMidEnd"))->Set(BleedDecay.MidEnd);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.DecayMidRate"))->Set(BleedDecay.MidDecay);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.DecayFarStart"))->Set(BleedDecay.FarStart);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.DecayFarEnd"))->Set(BleedDecay.FarEnd);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.DecayFarRate"))->Set(BleedDecay.FarDecay);

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("VegBleed: broadleaf=%.3f needle=%.3f shrub=%.3f talltree=%.3f decay=%.0f/%.0f/%.0f%%"),
		BroadleafIntensity, NeedleleafIntensity, ShrubIntensity, TallTreeIntensity,
		BleedDecay.NearDecay * 100.0f, BleedDecay.MidDecay * 100.0f, BleedDecay.FarDecay * 100.0f);
}

// ============================================================================
// v5.0 Wall Bleed - surface roughness, sun angle, downward projection only
// ============================================================================
void AGZGameMode::ApplyWallBleedParams(float SunAzimuth, float SunElevation)
{
	float EffectiveBleed = WallBleed.BaseBleedIntensity;

	// Direct sun: +0.09, side light: -0.06
	float DirectSunFactor = FMath::Clamp(FMath::Cos(FMath::DegreesToRadians(FMath::Abs(SunAzimuth - 90.0f))), 0.0f, 1.0f);
	EffectiveBleed += DirectSunFactor * WallBleed.DirectSunBoost;
	EffectiveBleed -= (1.0f - DirectSunFactor) * WallBleed.SideLightReduction;

	// Rough brick: +0.08 spread, smooth tile: focused
	float RoughnessSpread = (WallBleed.SurfaceType == EGZSurfaceRoughness::RoughBrick) ? WallBleed.RoughBleedSpread : WallBleed.SmoothTileFocus;

	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.WallIntensity"))->Set(EffectiveBleed);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.WallProjectionAngle"))->Set(WallBleed.ProjectionAngle);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.WallRoughnessSpread"))->Set(RoughnessSpread);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.WallSurfaceOcclusion"))->Set(WallBleed.bOcclusionFromSurface ? 1 : 0);

	UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("WallBleed: effective=%.3f projAngle=%.0f° spread=%.3f occlusion=%d"),
		EffectiveBleed, WallBleed.ProjectionAngle, RoughnessSpread, WallBleed.bOcclusionFromSurface);
}

// ============================================================================
// v5.0 Road Bleed - asphalt only receives, brightness reduction, wear-based
// ============================================================================
void AGZGameMode::ApplyRoadBleedParams()
{
	// Asphalt emits 0, only receives.
	// RoadWear 0-0.3 = new road (higher reflection); 0.3-1.0 = worn road (+0.05 absorption, -0.03 brightness)
	float EffectiveReceive = RoadBleed.ReceiveBleedIntensity;
	if (RoadBleed.RoadWear > RoadBleed.RoadWearThreshold)
	{
		EffectiveReceive += RoadBleed.WornRoadAbsorptionBoost;
	}
	else
	{
		EffectiveReceive += RoadBleed.NewRoadContrastBoost;
	}

	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.RoadIntensity"))->Set(EffectiveReceive);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.RoadEmitIntensity"))->Set(RoadBleed.EmitBleedIntensity);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.RoadBrightnessReduction"))->Set(RoadBleed.BrightnessReduction);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.ColorBleed.RoadWear"))->Set(RoadBleed.RoadWear);

	UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("RoadBleed: receive=%.3f emit=%.3f brightness=-%.3f wetness=%.2f"),
		EffectiveReceive, RoadBleed.EmitBleedIntensity, RoadBleed.BrightnessReduction, RoadWetness);
}

// ============================================================================
// v5.0 TSR Distance Weights - 7 tiers, per-distance frame accumulation
// High-speed movement: far-distance frame cache locked to 6 frames, near reduced
// ============================================================================
void AGZGameMode::ApplyTSRDistanceWeights(float CameraDistance, float VehicleSpeedKmh)
{
	EGZTSRDistanceTier Tier;
	if (CameraDistance < 5000.0f) Tier = EGZTSRDistanceTier::Near_0_50;
	else if (CameraDistance < 10000.0f) Tier = EGZTSRDistanceTier::MidNear_50_100;
	else if (CameraDistance < 20000.0f) Tier = EGZTSRDistanceTier::Mid_100_200;
	else if (CameraDistance < 40000.0f) Tier = EGZTSRDistanceTier::MidFar_200_400;
	else if (CameraDistance < 80000.0f) Tier = EGZTSRDistanceTier::Far_400_800;
	else if (CameraDistance < 150000.0f) Tier = EGZTSRDistanceTier::VeryFar_800_1500;
	else Tier = EGZTSRDistanceTier::Skyline_1500Plus;

	// Tier weights: 0-50m=0.0, 50-100m=0.05, 100-200m=0.08, 200-400m=0.12, 400-800m=0.15, 800-1500m=0.18, 1500m+=0.20
	float TierWeights[] = {0.0f, 0.05f, 0.08f, 0.12f, 0.15f, 0.18f, 0.20f};
	float Weight = TierWeights[(int32)Tier];

	// Far-distance frame cache locked to 6 frames during high-speed movement (>60km/h)
	bool bHighSpeed = VehicleSpeedKmh > AdaptiveResolution.VehicleHighSpeedThreshold;
	int32 FrameCache = TSRDistanceWeights[(int32)Tier].FrameCacheCount;
	if (bHighSpeed && (int32)Tier >= (int32)EGZTSRDistanceTier::Far_400_800)
	{
		FrameCache = 6;
	}
	else if (bHighSpeed)
	{
		FrameCache = FMath::Max(FrameCache - 2, 4);
	}

	IConsoleManager::Get().FindConsoleVariable(TEXT("r.TSR.HistoryWeightBoost"))->Set(Weight);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.TSR.DistanceTier"))->Set((int32)Tier);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.TSR.FrameCacheCount"))->Set(FrameCache);

	UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("TSR: tier=%d dist=%.0f weight=%.2f cache=%d highSpeed=%d"),
		(int32)Tier, CameraDistance, Weight, FrameCache, bHighSpeed ? 1 : 0);
}

// ============================================================================
// v5.0 TSR Texture Sharpening - linear texture detection, per-type, light-aware
// ============================================================================
void AGZGameMode::ApplyTSRTextureSharpening(float AmbientLightLevel)
{
	float EffectiveSharpening = TSRSharpening.BaseSharpening;

	// Light level adaptation
	if (AmbientLightLevel > 0.7f)
	{
		EffectiveSharpening *= TSRSharpening.BrightLightReduction;
	}
	else if (AmbientLightLevel < 0.3f)
	{
		EffectiveSharpening *= TSRSharpening.LowLightBoost;
	}

	IConsoleManager::Get().FindConsoleVariable(TEXT("r.TSR.MetalSharpening.Base"))->Set(EffectiveSharpening);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.TSR.MetalSharpening.LinearBoost"))->Set(TSRSharpening.LinearTextureBoost);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.TSR.MetalSharpening.LinearMinPx"))->Set(TSRSharpening.LinearTextureMinPx);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.TSR.MetalSharpening.LinearMaxPx"))->Set(TSRSharpening.LinearTextureMaxPx);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.TSR.MetalSharpening.TrafficSign"))->Set(TSRSharpening.TrafficSignSharpening);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.TSR.MetalSharpening.BuildingText"))->Set(TSRSharpening.BuildingTextSharpening);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.TSR.MetalSharpening.RoadTexture"))->Set(TSRSharpening.RoadTextureSharpening);

	UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("TSRSharpening: base=%.3f linear=%.3f lightLevel=%.2f"),
		EffectiveSharpening, TSRSharpening.LinearTextureBoost, AmbientLightLevel);
}

// ============================================================================
// v5.0 Glass - Multi-layer refraction, refined Fresnel, color neutrality
// ============================================================================
void AGZGameMode::ApplyGlassRefractionParams()
{
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Glass.Layer1IOR"))->Set(GlassLayerParams.Layer1IOR);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Glass.Layer1BrightnessDecay"))->Set(GlassLayerParams.Layer1BrightnessDecay);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Glass.Layer2IOR"))->Set(GlassLayerParams.Layer2IOR);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Glass.Layer2BrightnessDecay"))->Set(GlassLayerParams.Layer2BrightnessDecay);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Glass.Layer2RefractionReduction"))->Set(GlassLayerParams.Layer2RefractionReduction);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Glass.AirGapDecay"))->Set(GlassLayerParams.AirGapDecay);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Glass.TotalContrastReduction"))->Set(GlassLayerParams.TotalContrastReduction);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Glass.FrameSmoothPixels"))->Set(GlassLayerParams.FrameSmoothPixels);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Glass.MaxLayers"))->Set(GlassLayerParams.MaxLayers);

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("GlassMultiLayer: IOR1=%.2f IOR2=%.2f decay=%.2f/%.2f contrast=-%.2f maxLayers=%d"),
		GlassLayerParams.Layer1IOR, GlassLayerParams.Layer2IOR,
		GlassLayerParams.Layer1BrightnessDecay, GlassLayerParams.Layer2BrightnessDecay,
		GlassLayerParams.TotalContrastReduction, GlassLayerParams.MaxLayers);
}

void AGZGameMode::ApplyRefinedFresnelParams()
{
	// Refined curve: 0°=0.05, 60°=0.15 slow, 85°=0.55 fast, 90°=0.74 cap
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Glass.Fresnel.View0"))->Set(FresnelParams.View0Reflect);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Glass.Fresnel.View60"))->Set(FresnelParams.View60Reflect);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Glass.Fresnel.View85"))->Set(FresnelParams.View85Reflect);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Glass.Fresnel.View90Cap"))->Set(FresnelParams.View90Reflect);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Glass.Fresnel.SunHighlightShrink"))->Set(FresnelParams.SunHighlightShrink);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Glass.Fresnel.SunHighlightBoost"))->Set(FresnelParams.SunHighlightBoost);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Glass.Fresnel.RoughnessBind"))->Set(FresnelParams.bRoughnessBindsFresnel ? 1 : 0);

	// Color neutrality: no blue filter, max 50K shift, only thick bottom glass
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Glass.ColorNeutrality.RemoveBlue"))->Set(GlassNeutrality.bRemoveBlueFilter ? 1 : 0);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Glass.ColorNeutrality.ThickBottom"))->Set(GlassNeutrality.ThickBottomThreshold);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Glass.ColorNeutrality.MaxShiftKelvin"))->Set(GlassNeutrality.MaxColorShiftKelvin);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Glass.ColorNeutrality.DuskShift"))->Set(GlassNeutrality.DuskColorShift);

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("GlassFresnel: 0°=%.2f 60°=%.2f 85°=%.2f 90°=%.2f highlight=%.2f/%.2f colorNeutral=%d"),
		FresnelParams.View0Reflect, FresnelParams.View60Reflect, FresnelParams.View85Reflect, FresnelParams.View90Reflect,
		FresnelParams.SunHighlightShrink, FresnelParams.SunHighlightBoost, GlassNeutrality.bRemoveBlueFilter);
}

// ============================================================================
// v5.0 Road Wetness State Machine
// ============================================================================
void AGZGameMode::UpdateRoadWetness(float DeltaSeconds)
{
	if (RoadWetState == EGZRoadWetState::Dry && RoadWetnessTarget <= 0.01f) return;

	switch (RoadWetState)
	{
	case EGZRoadWetState::Transitioning:
	{
		float WetSpeed = 1.0f / RoadWetnessParams.WetTransitionTime;
		RoadWetness += DeltaSeconds * WetSpeed;
		if (RoadWetness >= RoadWetnessTarget)
		{
			RoadWetness = RoadWetnessTarget;
			if (RoadWetness >= 0.99f)
			{
				RoadWetState = EGZRoadWetState::Wet;
			}
			else
			{
				RoadWetState = EGZRoadWetState::Drying;
			}
		}
		break;
	}
	case EGZRoadWetState::Wet:
	{
		if (RoadWetnessTarget < 0.01f)
		{
			RoadWetState = EGZRoadWetState::Drying;
		}
		break;
	}
	case EGZRoadWetState::Drying:
	{
		float DryTime = (CurrentWeather == EGZWeatherType::Clear) ? RoadWetnessParams.DryTimeClear : RoadWetnessParams.DryTimeCloudy;
		float DrySpeed = 1.0f / DryTime;
		RoadWetness -= DeltaSeconds * DrySpeed;
		if (RoadWetness <= 0.0f)
		{
			RoadWetness = 0.0f;
			RoadWetState = EGZRoadWetState::Dry;
		}
		break;
	}
	default: break;
	}

	// Apply road reflectivity and roughness based on wetness
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Road.ReflectivityBoost"))->Set(RoadWetnessParams.WetReflectivityBoost * RoadWetness);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Road.RoughnessReduction"))->Set(RoadWetnessParams.WetRoughnessReduction * RoadWetness);
}

// ============================================================================
// v5.0 Vegetation Wind Dynamics
// ============================================================================
void AGZGameMode::UpdateVegetationWind(float DeltaSeconds)
{
	if (WindStrength <= 0.01f) return;

	WindGustTimer += DeltaSeconds;

	// Random wind gusts
	float GustFactor = FMath::PerlinNoise1D(WindGustTimer * 0.3f) * WindParams.WindGustAmplitude;
	float EffectiveWind = WindStrength * (1.0f + GustFactor);

	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Vegetation.WindStrength"))->Set(EffectiveWind);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Vegetation.BranchPeriodMin"))->Set(WindParams.BranchSwingPeriodMin);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Vegetation.BranchPeriodMax"))->Set(WindParams.BranchSwingPeriodMax);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Vegetation.LeafPeriodMin"))->Set(WindParams.LeafSwingPeriodMin);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Vegetation.LeafPeriodMax"))->Set(WindParams.LeafSwingPeriodMax);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Vegetation.LocalVariation"))->Set(WindParams.LocalWindVariation);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Vegetation.ThickBranchScale"))->Set(WindParams.ThickBranchSwingScale);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Vegetation.ThinBranchScale"))->Set(WindParams.ThinBranchSwingScale);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Signs.HeavyMetalScale"))->Set(WindParams.HeavyMetalSwingScale);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Signs.LightClothScale"))->Set(WindParams.LightClothSwingScale);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Signs.AngleRange"))->Set(WindParams.SignSwingAngleRange);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Signs.PeriodMin"))->Set(WindParams.SignSwingPeriodMin);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Signs.PeriodMax"))->Set(WindParams.SignSwingPeriodMax);
}

// ============================================================================
// v5.1 Cloud Tier Lighting
// ============================================================================
void AGZGameMode::ApplyCloudTierLighting(EGZCloudThickness Tier)
{
	if (CloudTiers.Num() < 3) return;

	const FCloudTierLighting& TierData = CloudTiers[(int32)Tier];

	IConsoleManager::Get().FindConsoleVariable(TEXT("r.AmbientLighting.TopLayerReduction"))->Set(TierData.TopAmbientReduction);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.AmbientLighting.ShadowVariationPeriod"))->Set(TierData.ShadowVariationPeriod);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.AmbientLighting.ShadowVariationAmplitude"))->Set(TierData.ShadowVariationAmplitude);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Cloud.FlowPeriodMin"))->Set(TierData.CloudFlowPeriodMin);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Cloud.FlowPeriodMax"))->Set(TierData.CloudFlowPeriodMax);

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("CloudTier: tier=%d topReduction=%.3f shadowPeriod=%.1f shadowAmp=%.3f flow=%.0f-%.0fs"),
		(int32)Tier, TierData.TopAmbientReduction, TierData.ShadowVariationPeriod, TierData.ShadowVariationAmplitude,
		TierData.CloudFlowPeriodMin, TierData.CloudFlowPeriodMax);
}

// ============================================================================
// v5.1 Ray Tracing Config with distance-tiered sample counts
// 0-50m = 32 samples, 50-200m = 16 samples, 200m+ = 8 samples
// ============================================================================
void AGZGameMode::ApplyRayTracingConfig()
{
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.GlobalIllumination"))->Set(RayTracingConfig.bEnableRTGI ? 1 : 0);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.Shadows"))->Set(RayTracingConfig.bEnableRTShadows ? 1 : 0);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.AmbientOcclusion"))->Set(RayTracingConfig.bEnableRTAO ? 1 : 0);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.Reflections"))->Set(RayTracingConfig.bEnableRTReflections ? 1 : 0);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.SamplesPerPixel"))->Set(RayTracingConfig.RTSamplesPerPixel);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.NearSamples"))->Set(RayTracingConfig.RTNearSamples);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.MidSamples"))->Set(RayTracingConfig.RTMidSamples);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.FarSamples"))->Set(RayTracingConfig.RTFarSamples);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.NearDistance"))->Set(RayTracingConfig.RTNearDistance);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.MidDistance"))->Set(RayTracingConfig.RTMidDistance);

	// Ray-traced contact shadows with tiered sample counts
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Shadow.ContactShadows"))->Set(RayTracingConfig.bEnableContactShadows ? 1 : 0);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Shadow.ContactShadows.RayTracing"))->Set(RayTracingConfig.bEnableContactShadows ? 1 : 0);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Shadow.ContactShadows.Samples"))->Set(RayTracingConfig.ContactShadowSamples);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.Shadow.NearSamples"))->Set(RayTracingConfig.RTShadowNearSamples);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.Shadow.MidSamples"))->Set(RayTracingConfig.RTShadowMidSamples);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.Shadow.FarSamples"))->Set(RayTracingConfig.RTShadowFarSamples);

	// Path-traced reflections: neon lights use higher sample count
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.PathTracing.Reflections"))->Set(RayTracingConfig.bEnablePathTracingReflections ? 1 : 0);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.Reflections.NearSamples"))->Set(RayTracingConfig.ReflectionNearSamples);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.Reflections.MidSamples"))->Set(RayTracingConfig.ReflectionMidSamples);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.Reflections.FarSamples"))->Set(RayTracingConfig.ReflectionFarSamples);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.Reflections.NeonLightSamples"))->Set(RayTracingConfig.NeonLightSamples);

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("RayTracingConfig: GI=%d Shadows=%d AO=%d Reflections=%d SPP=%d Near=%d Mid=%d Far=%d Contact=%d PathTrace=%d Neon=%d"),
		RayTracingConfig.bEnableRTGI, RayTracingConfig.bEnableRTShadows, RayTracingConfig.bEnableRTAO,
		RayTracingConfig.bEnableRTReflections, RayTracingConfig.RTSamplesPerPixel,
		RayTracingConfig.RTNearSamples, RayTracingConfig.RTMidSamples, RayTracingConfig.RTFarSamples,
		RayTracingConfig.bEnableContactShadows, RayTracingConfig.bEnablePathTracingReflections,
		RayTracingConfig.NeonLightSamples);
}

// ============================================================================
// v5.1 DirectStorage Config
// ============================================================================
void AGZGameMode::ApplyDirectStorageConfig()
{
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.DirectStorage.Enable"))->Set(DirectStorageConfig.bEnableDirectStorage ? 1 : 0);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.DirectStorage.PreloadPriority1Distance"))->Set(DirectStorageConfig.PreloadPriority1_Distance);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.DirectStorage.PreloadPriority2Distance"))->Set(DirectStorageConfig.PreloadPriority2_Distance);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.DirectStorage.PreloadPriority3Distance"))->Set(DirectStorageConfig.PreloadPriority3_Distance);

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("DirectStorage: enable=%d P1=%.0f P2=%.0f P3=%.0f"),
		DirectStorageConfig.bEnableDirectStorage, DirectStorageConfig.PreloadPriority1_Distance,
		DirectStorageConfig.PreloadPriority2_Distance, DirectStorageConfig.PreloadPriority3_Distance);
}

// ============================================================================
// v5.1 Fog Spatialization
// ============================================================================
void AGZGameMode::ApplyFogSpatialization(float Elevation)
{
	float FogBoost = 0.0f;
	if (Elevation < FogSpatialization.ElevationThreshold)
	{
		float T = 1.0f - (Elevation / FogSpatialization.ElevationThreshold);
		FogBoost = FogSpatialization.LowElevationBoost * T;
	}
	else
	{
		float T = (Elevation - FogSpatialization.ElevationThreshold) / FogSpatialization.ElevationThreshold;
		FogBoost = -FogSpatialization.HighElevationReduction * FMath::Min(T, 1.0f);
	}

	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Fog.ElevationBoost"))->Set(FogBoost);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Fog.CoolWavelengthDecay"))->Set(FogSpatialization.CoolWavelengthDecay);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Fog.WarmWavelengthRetention"))->Set(FogSpatialization.WarmWavelengthRetention);

	UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("FogSpatialization: elevation=%.0f boost=%.3f coolDecay=%.2f warmRet=%.2f"),
		Elevation, FogBoost, FogSpatialization.CoolWavelengthDecay, FogSpatialization.WarmWavelengthRetention);
}

// ============================================================================
// v5.1 Weather Pre-Transition
// ============================================================================
void AGZGameMode::ApplyWeatherPreTransition(float DeltaSeconds)
{
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Weather.PreTransitionTime"))->Set(WeatherPreTransition.PreTransitionTime);

	if (TargetWeather == EGZWeatherType::Storm)
	{
		float Darkening = WeatherPreTransition.StormDarkeningIntensity * WeatherTransitionProgress;
		IConsoleManager::Get().FindConsoleVariable(TEXT("r.Weather.StormDarkening"))->Set(Darkening);
	}

	if (WeatherPreTransition.TransitionCurveType == TEXT("easeInOut"))
	{
		IConsoleManager::Get().FindConsoleVariable(TEXT("r.Weather.TransitionCurve"))->Set(0);
	}
	else if (WeatherPreTransition.TransitionCurveType == TEXT("linear"))
	{
		IConsoleManager::Get().FindConsoleVariable(TEXT("r.Weather.TransitionCurve"))->Set(1);
	}

	UE_LOG(LogGuangzhouOpenWorld, Verbose, TEXT("WeatherPreTransition: progress=%.2f curve=%s darkening=%.2f"),
		WeatherTransitionProgress, *WeatherPreTransition.TransitionCurveType, WeatherPreTransition.StormDarkeningIntensity);
}

// ============================================================================
// v5.1 Nanite Seam Fix
// ============================================================================
void AGZGameMode::ApplyNaniteSeamFix()
{
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Nanite.SeamFix.VertexInterpolationDistance"))->Set(NaniteSeamFix.VertexInterpolationDistance);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Nanite.SeamFix.MaxSeamGap"))->Set(NaniteSeamFix.MaxSeamGap);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Nanite.SeamFix.EnableBlend"))->Set(NaniteSeamFix.bEnableSeamBlend ? 1 : 0);

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("NaniteSeamFix: interpDist=%.3f maxGap=%.3f blend=%d"),
		NaniteSeamFix.VertexInterpolationDistance, NaniteSeamFix.MaxSeamGap, NaniteSeamFix.bEnableSeamBlend);
}

// ============================================================================
// v5.1 Traffic Behavior
// ============================================================================
void AGZGameMode::ApplyTrafficBehavior(EGZTrafficBehavior Behavior)
{
	if (TrafficBehaviors.Num() < 5) return;

	const FTrafficAIExpanded& BehaviorData = TrafficBehaviors[(int32)Behavior];

	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Traffic.DecelerationBuffer"))->Set(BehaviorData.DecelerationBuffer);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Traffic.SmoothTurnRadius"))->Set(BehaviorData.SmoothTurnRadius);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Traffic.RainBrakingExtension"))->Set(BehaviorData.RainBrakingExtension);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Traffic.RainTurnRadiusIncrease"))->Set(BehaviorData.RainTurnRadiusIncrease);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Traffic.WaterSplashThreshold"))->Set(BehaviorData.WaterSplashThreshold);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Traffic.WaterSplashMaxHeight"))->Set(BehaviorData.WaterSplashMaxHeight);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Traffic.AutoLights"))->Set(BehaviorData.bEnableAutoLights ? 1 : 0);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Traffic.TunnelLightTrigger"))->Set(BehaviorData.TunnelLightTrigger);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.Traffic.NightLightTrigger"))->Set(BehaviorData.NightLightTrigger);

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("TrafficBehavior: behavior=%d decelBuf=%.1f turnRadius=%.1f rainBrake=%.1f rainTurn=%.1f splash=%.2f/%.0f autoLights=%d"),
		(int32)Behavior, BehaviorData.DecelerationBuffer, BehaviorData.SmoothTurnRadius,
		BehaviorData.RainBrakingExtension, BehaviorData.RainTurnRadiusIncrease,
		BehaviorData.WaterSplashThreshold, BehaviorData.WaterSplashMaxHeight, BehaviorData.bEnableAutoLights);
}

// ============================================================================
// v7.0 DLSS 4.5 / FSR 4 Config - UE5.8 latest protocols
// ============================================================================
void AGZGameMode::ApplyDLSSFSRConfig()
{
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.NGX.DLSS.Enable"))->Set(RayTracingConfig.bEnableDLSS ? 1 : 0);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.NGX.DLSS.Quality"))->Set(3);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.NGX.DLSS.FrameGeneration"))->Set(RayTracingConfig.bEnableFrameGen ? 1 : 0);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.NGX.DLSS.RayReconstruction"))->Set(RayTracingConfig.bEnableRTGI ? 1 : 0);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FSR.Enable"))->Set(RayTracingConfig.bEnableFSR ? 1 : 0);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FSR.Quality"))->Set(3);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FSR.FrameInterpolation"))->Set(RayTracingConfig.bEnableFrameGen ? 1 : 0);
	IConsoleManager::Get().FindConsoleVariable(TEXT("r.TSR.DLSSFSR.BothEnabled"))->Set((RayTracingConfig.bEnableDLSS && RayTracingConfig.bEnableFSR) ? 1 : 0);

	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("DLSSFSR: DLSS4.5=%d FSR4=%d FrameGen=%d"),
		RayTracingConfig.bEnableDLSS, RayTracingConfig.bEnableFSR, RayTracingConfig.bEnableFrameGen);
}
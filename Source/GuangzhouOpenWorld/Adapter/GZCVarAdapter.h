#pragma once

// GZCVarAdapter.h
// 自定义控制台变量安全适配层
// 原则：保留全部原有 CVar 定义，统一注册并增加空值保护

#include "CoreMinimal.h"
#include "HAL/IConsoleManager.h"

// 安全设置 CVar：空指针保护，不存在时自动注册并设置默认值
inline void GZSafeSetCVar(const TCHAR* Name, float Value, const TCHAR* Help = TEXT("GuangzhouOpenWorld custom CVar"), float DefaultValue = 0.0f)
{
	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(Name);
	if (!CVar)
	{
		CVar = IConsoleManager::Get().RegisterConsoleVariable(Name, DefaultValue, Help, ECVF_Default);
	}
	if (CVar)
	{
		CVar->Set(Value, EConsoleVariableFlags::ECVF_SetByGameSetting);
	}
}

inline void GZSafeSetCVarInt(const TCHAR* Name, int32 Value, const TCHAR* Help = TEXT("GuangzhouOpenWorld custom CVar"), int32 DefaultValue = 0)
{
	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(Name);
	if (!CVar)
	{
		CVar = IConsoleManager::Get().RegisterConsoleVariable(Name, DefaultValue, Help, ECVF_Default);
	}
	if (CVar)
	{
		CVar->Set(Value, EConsoleVariableFlags::ECVF_SetByGameSetting);
	}
}

inline void GZSafeSetCVarBool(const TCHAR* Name, bool bValue, const TCHAR* Help = TEXT("GuangzhouOpenWorld custom CVar"), bool bDefaultValue = false)
{
	GZSafeSetCVarInt(Name, bValue ? 1 : 0, Help, bDefaultValue ? 1 : 0);
}

// 注册全部 GuangzhouOpenWorld 自定义 CVar，供 GameMode 在 BeginPlay 中统一调用
inline void GZRegisterAllCustomCVars()
{
	// Rendering / Lumen / ColorBleed
	GZSafeSetCVar(TEXT("r.Lumen.ColorBleed.MaxDistance"), 800.0f);
	GZSafeSetCVar(TEXT("r.Lumen.ColorBleed.DecayMode"), 1.0f);
	GZSafeSetCVar(TEXT("r.Lumen.ColorBleed.WallIntensity"), 0.22f);
	GZSafeSetCVar(TEXT("r.Lumen.ColorBleed.GlassIntensity"), 0.08f);
	GZSafeSetCVar(TEXT("r.Lumen.ColorBleed.RoadIntensity"), 0.15f);
	GZSafeSetCVar(TEXT("r.Lumen.ColorBleed.VegetationIntensity"), 0.05f);
	GZSafeSetCVar(TEXT("r.Lumen.ColorBleed.RedWallIntensity"), 0.03f);

	// Weather
	GZSafeSetCVar(TEXT("r.Weather.Transition.DirectSunAlpha"), 0.0f);
	GZSafeSetCVar(TEXT("r.Weather.Transition.ColorTempAlpha"), 0.0f);
	GZSafeSetCVar(TEXT("r.Weather.Transition.ContrastAlpha"), 0.0f);
	GZSafeSetCVar(TEXT("r.Weather.Transition.RainColorTemp"), 7000.0f);
	GZSafeSetCVar(TEXT("r.Weather.Transition.ContrastDrop"), 0.4f);
	GZSafeSetCVar(TEXT("r.Weather.Transition.ShadowBlurBoost"), 1.0f);
	GZSafeSetCVar(TEXT("r.Weather.Transition.ShadowLengthScale"), 0.85f);

	// Fog
	GZSafeSetCVar(TEXT("r.Fog.SaturationNearNoDecay"), 2000.0f);
	GZSafeSetCVar(TEXT("r.Fog.SaturationMidDecay"), 5000.0f);
	GZSafeSetCVar(TEXT("r.Fog.SaturationFarDecay"), 8000.0f);
	GZSafeSetCVar(TEXT("r.Fog.FarBrightnessFloor"), 0.85f);
	GZSafeSetCVar(TEXT("r.Fog.ShadowBlurMultiplier"), 2.0f);
	GZSafeSetCVar(TEXT("r.Fog.ShadowLengthReduction"), 0.15f);
	GZSafeSetCVar(TEXT("r.Fog.LowElevationBoost"), 0.15f);
	GZSafeSetCVar(TEXT("r.Fog.HighElevationReduction"), 0.1f);
	GZSafeSetCVar(TEXT("r.Fog.CoolColorDecayFaster"), 1.3f);
	GZSafeSetCVar(TEXT("r.Fog.WarmColorRetention"), 0.8f);

	// Cell blend
	GZSafeSetCVar(TEXT("r.WorldPartition.CellBlend.WidthCells"), 2.0f);
	GZSafeSetCVar(TEXT("r.WorldPartition.CellBlend.WidthMeters"), 256.0f);
	GZSafeSetCVar(TEXT("r.WorldPartition.CellBlend.CosineCurve"), 1.0f);
	GZSafeSetCVar(TEXT("r.WorldPartition.CellBlend.ColorTempThreshold"), 300.0f);
	GZSafeSetCVar(TEXT("r.WorldPartition.CellBlend.ShadowHardness"), 1.0f);
	GZSafeSetCVar(TEXT("r.WorldPartition.CellBlend.ShadowDirection"), 1.0f);
	GZSafeSetCVar(TEXT("r.WorldPartition.CellBlend.BleedIntensity"), 1.0f);
	GZSafeSetCVar(TEXT("r.WorldPartition.CellBlend.AmbientOcclusion"), 1.0f);
	GZSafeSetCVar(TEXT("r.WorldPartition.LightingConsistency.MaxBrightnessDelta"), 0.05f);
	GZSafeSetCVar(TEXT("r.WorldPartition.LightingConsistency.MaxColorTempDelta"), 500.0f);
	GZSafeSetCVar(TEXT("r.WorldPartition.LightingConsistency.TransitionCells"), 2.0f);

	// Ambient lighting
	GZSafeSetCVar(TEXT("r.AmbientLighting.TopLayerBrightness"), 0.12f);
	GZSafeSetCVar(TEXT("r.AmbientLighting.TopLayerColorTemp"), 6800.0f);
	GZSafeSetCVar(TEXT("r.AmbientLighting.SideLayerBrightness"), 0.07f);
	GZSafeSetCVar(TEXT("r.AmbientLighting.SideLayerColorTemp"), 7200.0f);
	GZSafeSetCVar(TEXT("r.AmbientLighting.EdgeContrastRetention"), 0.70f);

	// TSR
	GZSafeSetCVar(TEXT("r.TSR.HistoryWeightBoost"), 0.0f);
	GZSafeSetCVar(TEXT("r.TSR.DistanceTier"), 0.0f);
	GZSafeSetCVar(TEXT("r.TSR.FrameCacheCount"), 8.0f);
	GZSafeSetCVar(TEXT("r.TSR.MetalSharpening.Base"), 0.18f);
	GZSafeSetCVar(TEXT("r.TSR.MetalSharpening.LinearBoost"), 0.08f);
	GZSafeSetCVar(TEXT("r.TSR.MetalSharpening.LinearMinPx"), 1.0f);
	GZSafeSetCVar(TEXT("r.TSR.MetalSharpening.LinearMaxPx"), 3.0f);
	GZSafeSetCVar(TEXT("r.TSR.MetalSharpening.TrafficSign"), 0.12f);
	GZSafeSetCVar(TEXT("r.TSR.MetalSharpening.BuildingText"), 0.08f);
	GZSafeSetCVar(TEXT("r.TSR.MetalSharpening.RoadTexture"), 0.04f);

	// Glass
	GZSafeSetCVar(TEXT("r.Glass.Layer1IOR"), 1.52f);
	GZSafeSetCVar(TEXT("r.Glass.Layer1BrightnessDecay"), 0.18f);
	GZSafeSetCVar(TEXT("r.Glass.Layer2IOR"), 1.52f);
	GZSafeSetCVar(TEXT("r.Glass.Layer2BrightnessDecay"), 0.12f);
	GZSafeSetCVar(TEXT("r.Glass.Layer2RefractionReduction"), 0.50f);
	GZSafeSetCVar(TEXT("r.Glass.AirGapDecay"), 0.03f);
	GZSafeSetCVar(TEXT("r.Glass.TotalContrastReduction"), 0.25f);
	GZSafeSetCVar(TEXT("r.Glass.FrameSmoothPixels"), 0.5f);
	GZSafeSetCVar(TEXT("r.Glass.MaxLayers"), 3.0f);
	GZSafeSetCVar(TEXT("r.Glass.Fresnel.View0"), 0.05f);
	GZSafeSetCVar(TEXT("r.Glass.Fresnel.View60"), 0.15f);
	GZSafeSetCVar(TEXT("r.Glass.Fresnel.View85"), 0.55f);
	GZSafeSetCVar(TEXT("r.Glass.Fresnel.View90Cap"), 0.70f);
	GZSafeSetCVar(TEXT("r.Glass.Fresnel.SunHighlightShrink"), 0.20f);
	GZSafeSetCVar(TEXT("r.Glass.Fresnel.SunHighlightBoost"), 0.15f);
	GZSafeSetCVar(TEXT("r.Glass.Fresnel.RoughnessBind"), 1.0f);
	GZSafeSetCVar(TEXT("r.Glass.ColorNeutrality.RemoveBlue"), 1.0f);
	GZSafeSetCVar(TEXT("r.Glass.ColorNeutrality.ThickBottom"), 0.35f);
	GZSafeSetCVar(TEXT("r.Glass.ColorNeutrality.MaxShiftKelvin"), 30.0f);
	GZSafeSetCVar(TEXT("r.Glass.ColorNeutrality.DuskShift"), 30.0f);

	// Road wetness
	GZSafeSetCVar(TEXT("r.Road.ReflectivityBoost"), 0.0f);
	GZSafeSetCVar(TEXT("r.Road.RoughnessReduction"), 0.0f);
	GZSafeSetCVar(TEXT("r.Road.PuddleReflectivityExtra"), 0.0f);
	GZSafeSetCVar(TEXT("r.Road.SurfaceDryScale"), 1.0f);
	GZSafeSetCVar(TEXT("r.Road.SunFacingDryFactor"), 1.0f);
	GZSafeSetCVar(TEXT("r.Road.PuddleAmount"), 0.0f);
	GZSafeSetCVar(TEXT("r.Road.PuddleDepth"), 0.0f);
	GZSafeSetCVar(TEXT("r.Road.RippleIntensity"), 0.0f);
	GZSafeSetCVar(TEXT("r.Road.RippleDirX"), 0.0f);
	GZSafeSetCVar(TEXT("r.Road.RippleDirY"), 0.0f);

	// Vegetation / wind / physics
	GZSafeSetCVar(TEXT("r.Vegetation.WindStrength"), 0.0f);
	GZSafeSetCVar(TEXT("r.Vegetation.BranchPeriodMin"), 8.0f);
	GZSafeSetCVar(TEXT("r.Vegetation.BranchPeriodMax"), 12.0f);
	GZSafeSetCVar(TEXT("r.Vegetation.LeafPeriodMin"), 2.0f);
	GZSafeSetCVar(TEXT("r.Vegetation.LeafPeriodMax"), 4.0f);
	GZSafeSetCVar(TEXT("r.Vegetation.LocalVariation"), 0.15f);
	GZSafeSetCVar(TEXT("r.Vegetation.ThickBranchScale"), 0.2f);
	GZSafeSetCVar(TEXT("r.Vegetation.ThinBranchScale"), 1.5f);
	GZSafeSetCVar(TEXT("r.Signs.HeavyMetalScale"), 0.3f);
	GZSafeSetCVar(TEXT("r.Signs.LightClothScale"), 2.0f);
	GZSafeSetCVar(TEXT("r.Signs.AngleRange"), 15.0f);
	GZSafeSetCVar(TEXT("r.Signs.PeriodMin"), 3.0f);
	GZSafeSetCVar(TEXT("r.Signs.PeriodMax"), 7.0f);
	GZSafeSetCVar(TEXT("r.PhysicsWind.Enable"), 1.0f);
	GZSafeSetCVar(TEXT("r.PhysicsWind.MaxSignAngle"), 15.0f);
	GZSafeSetCVar(TEXT("r.PhysicsWind.MaxBranchAngle"), 8.0f);
	GZSafeSetCVar(TEXT("r.PhysicsWind.MaxLeafAngle"), 25.0f);
	GZSafeSetCVar(TEXT("r.PhysicsWind.ForceMultiplier"), 100.0f);
	GZSafeSetCVar(TEXT("r.PhysicsWind.Damping"), 0.92f);

	// Ray tracing
	GZSafeSetCVar(TEXT("r.RayTracing.GlobalIllumination"), 1.0f);
	GZSafeSetCVar(TEXT("r.RayTracing.Shadows"), 1.0f);
	GZSafeSetCVar(TEXT("r.RayTracing.AmbientOcclusion"), 1.0f);
	GZSafeSetCVar(TEXT("r.RayTracing.Reflections"), 1.0f);
	GZSafeSetCVar(TEXT("r.RayTracing.SamplesPerPixel"), 1.0f);
	GZSafeSetCVar(TEXT("r.RayTracing.NearSamples"), 48.0f);
	GZSafeSetCVar(TEXT("r.RayTracing.MidSamples"), 24.0f);
	GZSafeSetCVar(TEXT("r.RayTracing.FarSamples"), 12.0f);
	GZSafeSetCVar(TEXT("r.RayTracing.NearDistance"), 5000.0f);
	GZSafeSetCVar(TEXT("r.RayTracing.MidDistance"), 20000.0f);
	GZSafeSetCVar(TEXT("r.Shadow.ContactShadows"), 1.0f);
	GZSafeSetCVar(TEXT("r.Shadow.ContactShadows.RayTracing"), 1.0f);
	GZSafeSetCVar(TEXT("r.Shadow.ContactShadows.Samples"), 24.0f);
	GZSafeSetCVar(TEXT("r.RayTracing.Shadow.NearSamples"), 48.0f);
	GZSafeSetCVar(TEXT("r.RayTracing.Shadow.MidSamples"), 24.0f);
	GZSafeSetCVar(TEXT("r.RayTracing.Shadow.FarSamples"), 12.0f);
	GZSafeSetCVar(TEXT("r.PathTracing.Reflections"), 1.0f);
	GZSafeSetCVar(TEXT("r.RayTracing.Reflections.NearSamples"), 48.0f);
	GZSafeSetCVar(TEXT("r.RayTracing.Reflections.MidSamples"), 24.0f);
	GZSafeSetCVar(TEXT("r.RayTracing.Reflections.FarSamples"), 12.0f);
	GZSafeSetCVar(TEXT("r.RayTracing.Reflections.NeonLightSamples"), 96.0f);

	// DirectStorage / streaming
	GZSafeSetCVar(TEXT("r.DirectStorage.Enable"), 1.0f);
	GZSafeSetCVar(TEXT("r.DirectStorage.PreloadPriority1Distance"), 512.0f);
	GZSafeSetCVar(TEXT("r.DirectStorage.PreloadPriority2Distance"), 384.0f);
	GZSafeSetCVar(TEXT("r.DirectStorage.PreloadPriority3Distance"), 256.0f);
	GZSafeSetCVar(TEXT("r.DirectStorage.AsyncIO"), 1.0f);
	GZSafeSetCVar(TEXT("r.DirectStorage.BypassOSCache"), 1.0f);
	GZSafeSetCVar(TEXT("r.Streaming.LandmarkPriority"), 3.0f);
	GZSafeSetCVar(TEXT("r.Streaming.BuildingPriority"), 2.0f);
	GZSafeSetCVar(TEXT("r.Streaming.PropPriority"), 1.0f);
	GZSafeSetCVar(TEXT("r.Streaming.LandmarkCullScale"), 2.0f);
	GZSafeSetCVar(TEXT("r.Streaming.BuildingCullScale"), 1.0f);
	GZSafeSetCVar(TEXT("r.Streaming.PropCullScale"), 0.6f);

	// Weather pre-transition / state
	GZSafeSetCVar(TEXT("r.Weather.PreTransitionTime"), 3.0f);
	GZSafeSetCVar(TEXT("r.Weather.StormDarkening"), 0.0f);
	GZSafeSetCVar(TEXT("r.Weather.TransitionCurve"), 0.0f);
	GZSafeSetCVar(TEXT("r.Weather.State.SunIntensityScale"), 1.0f);
	GZSafeSetCVar(TEXT("r.Weather.State.SkyIntensityScale"), 1.0f);
	GZSafeSetCVar(TEXT("r.Weather.State.ColorTempKelvin"), 6500.0f);
	GZSafeSetCVar(TEXT("r.Weather.State.FogDensityScale"), 1.0f);
	GZSafeSetCVar(TEXT("r.Weather.State.FogShadowBlurScale"), 1.0f);
	GZSafeSetCVar(TEXT("r.Weather.State.FogShadowLengthScale"), 1.0f);
	GZSafeSetCVar(TEXT("r.Weather.State.WindStrengthScale"), 1.0f);

	// Cloud tier / shadow
	GZSafeSetCVar(TEXT("r.AmbientLighting.TopLayerReduction"), 0.0f);
	GZSafeSetCVar(TEXT("r.AmbientLighting.ShadowVariationPeriod"), 4.0f);
	GZSafeSetCVar(TEXT("r.AmbientLighting.ShadowVariationAmplitude"), 0.08f);
	GZSafeSetCVar(TEXT("r.Cloud.FlowPeriodMin"), 15.0f);
	GZSafeSetCVar(TEXT("r.Cloud.FlowPeriodMax"), 45.0f);
	GZSafeSetCVar(TEXT("r.CloudShadow.Intensity"), 0.4f);
	GZSafeSetCVar(TEXT("r.CloudShadow.MoveSpeed"), 250.0f);
	GZSafeSetCVar(TEXT("r.CloudShadow.WindMultiplier"), 2.0f);
	GZSafeSetCVar(TEXT("r.CloudShadow.BlendTime"), 4.0f);
	GZSafeSetCVar(TEXT("r.CloudShadow.Offset"), 0.0f);
	GZSafeSetCVar(TEXT("r.CloudShadow.LowElevationBoost"), 0.2f);
	GZSafeSetCVar(TEXT("r.CloudShadow.ThicknessScale"), 1.25f);

	// Fog spatialization
	GZSafeSetCVar(TEXT("r.Fog.ElevationBoost"), 0.0f);
	GZSafeSetCVar(TEXT("r.Fog.CoolWavelengthDecay"), 1.2f);
	GZSafeSetCVar(TEXT("r.Fog.WarmWavelengthRetention"), 0.85f);

	// Nanite
	GZSafeSetCVar(TEXT("r.Nanite.QualityLevel"), 4.0f);
	GZSafeSetCVar(TEXT("r.Nanite.MaxPixelsPerEdge"), 0.25f);
	GZSafeSetCVar(TEXT("r.Nanite.MinMaterialDisplacement"), 0.125f);
	GZSafeSetCVar(TEXT("r.Nanite.SeamFix.VertexInterpolationDistance"), 0.01f);
	GZSafeSetCVar(TEXT("r.Nanite.SeamFix.MaxSeamGap"), 0.01f);
	GZSafeSetCVar(TEXT("r.Nanite.SeamFix.EnableBlend"), 1.0f);

	// Color bleed decay
	GZSafeSetCVar(TEXT("r.Lumen.ColorBleed.DecayNearStart"), 0.0f);
	GZSafeSetCVar(TEXT("r.Lumen.ColorBleed.DecayNearEnd"), 200.0f);
	GZSafeSetCVar(TEXT("r.Lumen.ColorBleed.DecayNearRate"), 0.6f);
	GZSafeSetCVar(TEXT("r.Lumen.ColorBleed.DecayMidStart"), 200.0f);
	GZSafeSetCVar(TEXT("r.Lumen.ColorBleed.DecayMidEnd"), 500.0f);
	GZSafeSetCVar(TEXT("r.Lumen.ColorBleed.DecayMidRate"), 0.3f);
	GZSafeSetCVar(TEXT("r.Lumen.ColorBleed.DecayFarStart"), 500.0f);
	GZSafeSetCVar(TEXT("r.Lumen.ColorBleed.DecayFarEnd"), 800.0f);
	GZSafeSetCVar(TEXT("r.Lumen.ColorBleed.DecayFarRate"), 0.1f);
	GZSafeSetCVar(TEXT("r.Lumen.ColorBleed.BroadleafIntensity"), 0.055f);
	GZSafeSetCVar(TEXT("r.Lumen.ColorBleed.NeedleleafIntensity"), 0.0425f);
	GZSafeSetCVar(TEXT("r.Lumen.ColorBleed.ShrubIntensity"), 0.01f);
	GZSafeSetCVar(TEXT("r.Lumen.ColorBleed.TallTreeIntensity"), 0.12f);
	GZSafeSetCVar(TEXT("r.Lumen.ColorBleed.SunAzimuthOffset"), 0.0f);
	GZSafeSetCVar(TEXT("r.Lumen.ColorBleed.WallProjectionAngle"), 30.0f);
	GZSafeSetCVar(TEXT("r.Lumen.ColorBleed.WallRoughnessSpread"), 0.08f);
	GZSafeSetCVar(TEXT("r.Lumen.ColorBleed.WallSurfaceOcclusion"), 1.0f);
	GZSafeSetCVar(TEXT("r.Lumen.ColorBleed.RoadEmitIntensity"), 0.0f);
	GZSafeSetCVar(TEXT("r.Lumen.ColorBleed.RoadBrightnessReduction"), 0.03f);
	GZSafeSetCVar(TEXT("r.Lumen.ColorBleed.RoadWear"), 0.0f);

	// District
	GZSafeSetCVar(TEXT("r.District.RoadWear"), 0.0f);
	GZSafeSetCVar(TEXT("r.District.RoughnessScale"), 1.0f);
	GZSafeSetCVar(TEXT("r.District.PuddleEvaporationScale"), 1.0f);
	GZSafeSetCVar(TEXT("r.District.VegetationWindScale"), 1.0f);
	GZSafeSetCVar(TEXT("r.District.VegetationDensity"), 1.0f);
	GZSafeSetCVar(TEXT("r.District.NeonScale"), 1.0f);
	GZSafeSetCVar(TEXT("r.District.PedestrianSpeedScale"), 1.0f);
	GZSafeSetCVar(TEXT("r.District.LandmarkVisibility"), 100000.0f);
	GZSafeSetCVar(TEXT("r.District.FarCoolTintR"), 0.75f);
	GZSafeSetCVar(TEXT("r.District.FarCoolTintG"), 0.85f);
	GZSafeSetCVar(TEXT("r.District.FarCoolTintB"), 1.0f);

	// Rendering v8.0
	GZSafeSetCVar(TEXT("r.Neon.BaseIntensity"), 1.0f);
	GZSafeSetCVar(TEXT("r.Neon.BleedDecay"), 2.6f);
	GZSafeSetCVar(TEXT("r.Water.DeepSampleDistance"), 300000.0f);
	GZSafeSetCVar(TEXT("r.Water.PuddleSampleDistance"), 50000.0f);
	GZSafeSetCVar(TEXT("r.Water.DeepDistortion"), 0.06f);
	GZSafeSetCVar(TEXT("r.Water.PuddleDistortion"), 0.22f);
	GZSafeSetCVar(TEXT("r.Water.VehicleUnderbodyVisibility"), 0.75f);
	GZSafeSetCVar(TEXT("r.Water.ShallowPuddleClarity"), 0.9f);
	GZSafeSetCVar(TEXT("r.Water.DeepPuddleClarity"), 0.5f);
	GZSafeSetCVar(TEXT("r.Water.VehicleDisturbanceRecovery"), 2.0f);
	GZSafeSetCVar(TEXT("r.Character.HairSamples"), 6.0f);
	GZSafeSetCVar(TEXT("r.Character.VolumetricScattering"), 0.45f);
	GZSafeSetCVar(TEXT("r.Character.SkinSubsurface"), 1.0f);
	GZSafeSetCVar(TEXT("r.Character.EnvLightBlend"), 1.0f);
	GZSafeSetCVar(TEXT("r.Atmospheric.NearDistance"), 0.0f);
	GZSafeSetCVar(TEXT("r.Atmospheric.FarDistance"), 120000.0f);
	GZSafeSetCVar(TEXT("r.Atmospheric.CoolBlueShift"), 0.3f);
	GZSafeSetCVar(TEXT("r.Atmospheric.WarmRetention"), 0.88f);
	GZSafeSetCVar(TEXT("r.MetallicRT.Enable"), 1.0f);
	GZSafeSetCVar(TEXT("r.MetallicRT.FresnelRangeScale"), 1.0f);
	GZSafeSetCVar(TEXT("r.MetallicRT.EnvLightResponse"), 1.1f);
	GZSafeSetCVar(TEXT("r.MetallicRT.FarFadeStart"), 50000.0f);
	GZSafeSetCVar(TEXT("r.MetallicRT.FarFadeEnd"), 150000.0f);
	GZSafeSetCVar(TEXT("r.Glass.OverlapBlendRadius"), 500.0f);
	GZSafeSetCVar(TEXT("r.Glass.OverlapColorDamping"), 0.55f);
	GZSafeSetCVar(TEXT("r.Glass.MaxOverlappingFacades"), 4.0f);
	GZSafeSetCVar(TEXT("r.Glass.ResolveColorConflict"), 1.0f);

	// Flood
	GZSafeSetCVar(TEXT("r.Flood.RainToWaterRate"), 0.5f);
	GZSafeSetCVar(TEXT("r.Flood.MaxHeight"), 50.0f);
	GZSafeSetCVar(TEXT("r.Flood.EvaporationRate"), 0.05f);
	GZSafeSetCVar(TEXT("r.Flood.VehicleSpeedPenalty"), 0.4f);
	GZSafeSetCVar(TEXT("r.Flood.NPCRerouteThreshold"), 20.0f);
	GZSafeSetCVar(TEXT("r.Flood.CurrentHeight"), 0.0f);

	// Traffic
	GZSafeSetCVar(TEXT("r.Traffic.DecelerationBuffer"), 0.0f);
	GZSafeSetCVar(TEXT("r.Traffic.SmoothTurnRadius"), 0.0f);
	GZSafeSetCVar(TEXT("r.Traffic.RainBrakingExtension"), 0.0f);
	GZSafeSetCVar(TEXT("r.Traffic.RainTurnRadiusIncrease"), 0.0f);
	GZSafeSetCVar(TEXT("r.Traffic.WaterSplashThreshold"), 0.0f);
	GZSafeSetCVar(TEXT("r.Traffic.WaterSplashMaxHeight"), 0.0f);
	GZSafeSetCVar(TEXT("r.Traffic.AutoLights"), 0.0f);
	GZSafeSetCVar(TEXT("r.Traffic.TunnelLightTrigger"), 0.0f);
	GZSafeSetCVar(TEXT("r.Traffic.NightLightTrigger"), 0.0f);
}

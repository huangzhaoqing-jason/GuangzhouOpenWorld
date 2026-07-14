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
}

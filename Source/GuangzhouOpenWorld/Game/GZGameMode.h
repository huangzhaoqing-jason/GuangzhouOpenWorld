#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GZGameMode.generated.h"

UENUM(BlueprintType)
enum class EAppleSiliconChip : uint8
{
	Unknown		UMETA(DisplayName = "Unknown"),
	M1			UMETA(DisplayName = "M1"),
	M2			UMETA(DisplayName = "M2"),
	M3			UMETA(DisplayName = "M3"),
};

UENUM(BlueprintType)
enum class EGZWeatherType : uint8
{
	Clear		UMETA(DisplayName = "Clear"),
	Cloudy		UMETA(DisplayName = "Cloudy"),
	Rain		UMETA(DisplayName = "Rain"),
	Storm		UMETA(DisplayName = "Storm"),
	FogHaze		UMETA(DisplayName = "Fog / Haze"),
};

UENUM(BlueprintType)
enum class EGZLightingZone : uint8
{
	OutdoorStreet	UMETA(DisplayName = "Outdoor Street"),
	Indoor			UMETA(DisplayName = "Indoor"),
	Tunnel			UMETA(DisplayName = "Tunnel"),
	RiverSurface	UMETA(DisplayName = "River Surface"),
};

UENUM(BlueprintType)
enum class EGZVegetationType : uint8
{
	Broadleaf	UMETA(DisplayName = "Broadleaf"),
	Needleleaf	UMETA(DisplayName = "Needleleaf"),
	Shrub		UMETA(DisplayName = "Shrub"),
	TallTree	UMETA(DisplayName = "Tall Tree"),
};

UENUM(BlueprintType)
enum class EGZSurfaceRoughness : uint8
{
	RoughBrick		UMETA(DisplayName = "Rough Brick"),
	SmoothTile		UMETA(DisplayName = "Smooth Tile"),
	WornAsphalt		UMETA(DisplayName = "Worn Asphalt"),
	NewAsphalt		UMETA(DisplayName = "New Asphalt"),
};

UENUM(BlueprintType)
enum class EGZRoadWetState : uint8
{
	Dry				UMETA(DisplayName = "Dry"),
	Transitioning	UMETA(DisplayName = "Transitioning"),
	Wet				UMETA(DisplayName = "Wet"),
	Drying			UMETA(DisplayName = "Drying"),
};

UENUM(BlueprintType)
enum class EGZTSRDistanceTier : uint8
{
	Near_0_50		UMETA(DisplayName = "0-50m"),
	MidNear_50_100	UMETA(DisplayName = "50-100m"),
	Mid_100_200		UMETA(DisplayName = "100-200m"),
	MidFar_200_400	UMETA(DisplayName = "200-400m"),
	Far_400_800		UMETA(DisplayName = "400-800m"),
	VeryFar_800_1500 UMETA(DisplayName = "800-1500m"),
	Skyline_1500Plus UMETA(DisplayName = "1500m+"),
};

UENUM(BlueprintType)
enum class EGZCloudThickness : uint8
{
	Heavy		UMETA(DisplayName = "Heavy"),
	Medium		UMETA(DisplayName = "Medium"),
	Thin		UMETA(DisplayName = "Thin"),
};

UENUM(BlueprintType)
enum class EGZTrafficBehavior : uint8
{
	Normal		UMETA(DisplayName = "Normal"),
	RushHour	UMETA(DisplayName = "RushHour"),
	NightSparse	UMETA(DisplayName = "NightSparse"),
	RainCautious UMETA(DisplayName = "RainCautious"),
	FogSlow		UMETA(DisplayName = "FogSlow"),
};

USTRUCT(BlueprintType)
struct FDayNightCycleSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float TimeOfDay = 8.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float DayLengthSeconds = 1440.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SunAngle = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SunIntensity = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float NightIntensity = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FLinearColor DayColor = FLinearColor(1.0f, 0.95f, 0.8f, 1.0f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FLinearColor NightColor = FLinearColor(0.1f, 0.15f, 0.4f, 1.0f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FLinearColor SunsetColor = FLinearColor(1.0f, 0.5f, 0.2f, 1.0f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FLinearColor SunriseColor = FLinearColor(0.9f, 0.6f, 0.3f, 1.0f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SunriseStart = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SunriseEnd = 7.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SunsetStart = 17.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SunsetEnd = 19.0f;
};

USTRUCT(BlueprintType)
struct FAutoQualitySettings
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ScreenPercentage = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 ShadowResolution = 2048;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 VolumetricFogGridSize = 128;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 SSRQuality = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BloomIntensity = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 TSRFrameCount = 8;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 NaniteQualityLevel = 4;
};

USTRUCT(BlueprintType)
struct FAdaptiveResolutionSettings
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float CBDScreenPercentage = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float StreetScreenPercentage = 90.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float FarScreenPercentage = 75.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float VehicleHighSpeedScreenPercentage = 95.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float VehicleHighSpeedThreshold = 60.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float TransitionTime = 0.2f;
};

// ============================================================================
// v5.0 Refined Color Bleed - Per-Vegetation, Per-Surface, Gradient Decay
// ============================================================================
USTRUCT(BlueprintType)
struct FVegetationBleedParams
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EGZVegetationType VegType = EGZVegetationType::Broadleaf;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BaseBleedIntensity = 0.05f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float CanopyBleedBoost = 0.07f;      // Tall tree canopy +0.07
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ShrubBleedReduction = 0.04f;    // Shrubs -0.04
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BroadleafRangeBoost = 1.10f;    // Broadleaf range +10%
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float NeedleleafIntensityScale = 0.85f; // Needleleaf -15%
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SunAngleOffset = 0.0f;           // Bleed offset follows sun azimuth
};

USTRUCT(BlueprintType)
struct FWallBleedParams
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EGZSurfaceRoughness SurfaceType = EGZSurfaceRoughness::RoughBrick;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BaseBleedIntensity = 0.22f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float DirectSunBoost = 0.09f;        // +0.09 when sun hits
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SideLightReduction = 0.06f;     // -0.06 side light
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ProjectionAngle = 30.0f;        // Only project downward 30°
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float RoughBleedSpread = 0.08f;       // Rough brick +0.08 spread
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SmoothTileFocus = 0.0f;         // Smooth tile focused
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bOcclusionFromSurface = true;    // Cavities block bleed
};

USTRUCT(BlueprintType)
struct FRoadBleedParams
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ReceiveBleedIntensity = 0.15f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float EmitBleedIntensity = 0.0f;      // Asphalt emits nothing
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BrightnessReduction = 0.03f;    // Bleed zone -0.03 brightness
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float WornRoadAbsorptionBoost = 0.05f; // Worn road +0.05
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float NewRoadContrastBoost = 0.02f;   // New road contrast+
};

// Gradient decay: 0-2m=60% fast, 2-5m=30% slow, 5-8m=10% tail
USTRUCT(BlueprintType)
struct FBleedDecayCurve
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float NearStart = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float NearEnd = 200.0f;       // 0-2m
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float NearDecay = 0.60f;      // 60% decay
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MidStart = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MidEnd = 500.0f;        // 2-5m
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MidDecay = 0.30f;       // 30% decay
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float FarStart = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float FarEnd = 800.0f;        // 5-8m
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float FarDecay = 0.10f;       // 10% tail to zero
};

// ============================================================================
// v5.0 World Partition Cell Blend - 2-cell width, cosine curve, full parameter blend
// ============================================================================
USTRUCT(BlueprintType)
struct FCellBlendRegion
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 BlendWidthCells = 2;               // 2 full cells
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BlendWidthMeters = 256.0f;          // 2×128m
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bUseCosineCurve = true;              // Quartic smooth
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ColorTempBlendThreshold = 300.0f;   // >300K triggers blend
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bBlendShadowHardness = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bBlendShadowDirection = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bBlendBleedIntensity = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bBlendAmbientOcclusion = true;
};

// ============================================================================
// v5.0 Weather Layered Lighting
// ============================================================================
USTRUCT(BlueprintType)
struct FWeatherLightingLayers
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float TopAmbientBrightness = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float TopAmbientColorTemp = 6800.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SideAmbientBrightness = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SideAmbientColorTemp = 7200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float EdgeContrastRetention = 0.70f;     // Keep 70% contour contrast
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float VisibilityNearNoDecay = 4000.0f;   // 0-40m no decay
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float VisibilityFarDecay = 8000.0f;      // 40-80m decay to 85%
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float FarBrightnessFloor = 0.85f;
};

USTRUCT(BlueprintType)
struct FFogLightingParams
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SatNearNoDecay = 2000.0f;     // 0-20m
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SatMidDecay = 5000.0f;        // 20-50m, -20% sat
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SatFarDecay = 8000.0f;        // 50-80m, -45% sat
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ShadowBlurMultiplier = 2.0f;  // Shadows 2x softer
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ShadowLengthReduction = 0.15f; // 15% shorter
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float LowElevationFogBoost = 0.15f; // Dense fog in low areas
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float CoolColorDecayFaster = 1.3f;  // Cool colors decay faster
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float WarmColorRetention = 0.8f;    // Warm colors retained
};

USTRUCT(BlueprintType)
struct FWeatherTransitionPhases
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Phase1_DirectSunReduction = 1.2f;   // 0-1.2s
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Phase2_ColorTempShift = 0.8f;       // 1.2-2.0s
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Phase3_ContrastShadow = 0.8f;       // 2.0-2.8s
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float CloudPreAnimation = 1.0f;           // 1s before
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bStormRapidDarkening = true;
};

// ============================================================================
// v5.0 Road Wetness State Machine
// ============================================================================
USTRUCT(BlueprintType)
struct FRoadWetnessParams
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float WetTransitionTime = 30.0f;       // 30s to full wet
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float DryTimeClear = 480.0f;           // 480s clear
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float DryTimeCloudy = 900.0f;          // 900s cloudy
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float WetReflectivityBoost = 0.22f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float WetRoughnessReduction = 0.16f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float PuddleReflectivityExtra = 0.20f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float PuddleEvapTimeScale = 0.70f;      // 30% faster than road
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SunFacingDrySpeedBoost = 1.40f;   // +40%
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ShadeFacingDrySpeedReduction = 0.80f; // -20%
};

// ============================================================================
// v5.0 Vegetation Wind Dynamics
// ============================================================================
USTRUCT(BlueprintType)
struct FVegetationWindParams
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BranchSwingPeriodMin = 8.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BranchSwingPeriodMax = 12.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float LeafSwingPeriodMin = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float LeafSwingPeriodMax = 4.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float WindGustAmplitude = 0.3f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float HeavyMetalSwingScale = 0.3f;    // Heavy signs
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float LightClothSwingScale = 2.0f;    // Light banners
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ThickBranchSwingScale = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ThinBranchSwingScale = 1.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float LocalWindVariation = 0.15f;      // ±15% local variation
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SignSwingAngleRange = 15.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SignSwingPeriodMin = 3.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SignSwingPeriodMax = 7.0f;
};

// ============================================================================
// v5.0 TSR Distance-Based Frame Weight Tiers
// ============================================================================
USTRUCT(BlueprintType)
struct FTSRDistanceWeight
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EGZTSRDistanceTier Tier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float HistoryWeightBoost = 0.0f;   // Additional weight over base
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MinDistance = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxDistance = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 FrameCacheCount = 8;
};

USTRUCT(BlueprintType)
struct FTSRTextureSharpening
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BaseSharpening = 0.18f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float LinearTextureBoost = 0.08f;    // Extra for 1-3px linear textures
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 LinearTextureMinPx = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 LinearTextureMaxPx = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float TrafficSignSharpening = 0.12f;  // Highest
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BuildingTextSharpening = 0.08f; // Medium
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float RoadTextureSharpening = 0.04f;  // Low
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BrightLightReduction = 0.7f;    // Reduce in bright light
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float LowLightBoost = 1.2f;           // Boost in low light
};

// ============================================================================
// v5.0 Glass Refraction - Multi-layer, refined Fresnel, color-neutral
// ============================================================================
USTRUCT(BlueprintType)
struct FMultiLayerGlassParams
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Layer1IOR = 1.52f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Layer1BrightnessDecay = 0.18f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Layer2IOR = 1.52f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Layer2BrightnessDecay = 0.12f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Layer2RefractionReduction = 0.50f; // Half offset
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float AirGapDecay = 0.03f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float TotalContrastReduction = 0.25f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float FrameSmoothPixels = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 MaxLayers = 3;
};

USTRUCT(BlueprintType)
struct FRefinedFresnelParams
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float View0Reflect = 0.05f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float View60Reflect = 0.15f;      // Slow 0-60°
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float View85Reflect = 0.55f;      // Fast 60-85°
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float View90Reflect = 0.74f;      // Cap at 90° (was 0.82)
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SunHighlightShrink = 0.20f;  // 20% smaller highlight
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SunHighlightBoost = 0.15f;   // 15% brighter
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bRoughnessBindsFresnel = true;
};

USTRUCT(BlueprintType)
struct FGlassColorNeutrality
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bRemoveBlueFilter = true;     // No blue tint
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ThickBottomThreshold = 0.35f; // mm
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxColorShiftKelvin = 50.0f;  // Imperceptible
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float DuskColorShift = 30.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float EdgeThicknessColorFade = 0.05f;
};

// Keep legacy structs for compatibility
USTRUCT(BlueprintType)
struct FLumenColorBleedSettings
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float WallBleed = 0.22f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float GlassBleed = 0.08f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float RoadBleed = 0.15f;
};

USTRUCT(BlueprintType)
struct FDayNightCausalityEntry
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float HourStart = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float HourEnd = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ColorTempKelvin = 6500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ShadowLengthMultiplier = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ShadowSoftness = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FLinearColor LightColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString PeriodName;
};

USTRUCT(BlueprintType)
struct FColorBleedCausality
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxBleedDistance = 800.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString BleedDecayCurve = TEXT("linear");
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float WallBleed = 0.22f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float GlassBleed = 0.08f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float RoadBleed = 0.15f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float VegetationBleed = 0.05f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float RedWallBleed = 0.03f;
};

USTRUCT(BlueprintType)
struct FCellLightingConsistency
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxBrightnessDelta = 0.05f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxColorTempDelta = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 TransitionZoneCells = 2;
};

// ============================================================================
// v5.1 Cloud Tier Lighting, Ray Tracing, DirectStorage, NPC, Traffic, Fog, Nanite
// ============================================================================
USTRUCT(BlueprintType)
struct FCloudTierLighting
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float TopAmbientReduction = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ShadowVariationPeriod = 4.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ShadowVariationAmplitude = 0.15f;
};

USTRUCT(BlueprintType)
struct FRayTracingConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bEnableRTGI = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bEnableRTShadows = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bEnableRTAO = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bEnableRTReflections = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bEnableDLSS = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bEnableFSR = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bEnableFrameGen = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 RTSamplesPerPixel = 1;
};

USTRUCT(BlueprintType)
struct FDirectStorageConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bEnableDirectStorage = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float PreloadPriority1_Distance = 4.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float PreloadPriority2_Distance = 3.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float PreloadPriority3_Distance = 2.0f;
};

USTRUCT(BlueprintType)
struct FNPC24HLifecycle
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bEnableMemory = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MemoryRetentionHours = 24.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bEnableInterNPCInteraction = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float InteractionRadius = 3000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float InteractionFrequencyMin = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float InteractionFrequencyMax = 60.0f;
};

USTRUCT(BlueprintType)
struct FTrafficAIExpanded
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float DecelerationBuffer = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SmoothTurnRadius = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float RainBrakingExtension = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float RainTurnRadiusIncrease = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float WaterSplashThreshold = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float WaterSplashMaxHeight = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bEnableAutoLights = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float TunnelLightTrigger = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float NightLightTrigger = 0.0f;
};

USTRUCT(BlueprintType)
struct FFogSpatialization
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float LowElevationBoost = 0.15f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float HighElevationReduction = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float CoolWavelengthDecay = 1.3f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float WarmWavelengthRetention = 0.8f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ElevationThreshold = 5000.0f;
};

USTRUCT(BlueprintType)
struct FWeatherPreTransition
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float PreTransitionTime = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float StormDarkeningIntensity = 0.3f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString TransitionCurveType = TEXT("easeInOut");
};

USTRUCT(BlueprintType)
struct FNaniteSeamFix
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float VertexInterpolationDistance = 0.01f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxSeamGap = 0.01f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bEnableSeamBlend = true;
};

UCLASS()
class GUANGZHOUOPENWORLD_API AGZGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGZGameMode();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable) EAppleSiliconChip DetectAppleSiliconChip() const;
	UFUNCTION(BlueprintCallable) void ApplyChipSpecificSettings();
	UFUNCTION(BlueprintCallable) void SetWeather(EGZWeatherType NewWeather);
	UFUNCTION(BlueprintPure) EGZWeatherType GetCurrentWeather() const { return CurrentWeather; }
	UFUNCTION(BlueprintCallable) void SetLightingZone(EGZLightingZone NewZone);
	UFUNCTION(BlueprintPure) EGZLightingZone GetCurrentLightingZone() const { return CurrentLightingZone; }
	UFUNCTION(BlueprintPure) float GetTimeOfDay() const { return DayNight.TimeOfDay; }
	UFUNCTION(BlueprintCallable) void SetTimeOfDay(float NewTime);
	UFUNCTION(BlueprintCallable) void UpdateAdaptiveResolution(float VehicleSpeedKmh, bool bIsInCBD, bool bIsInStreet, bool bIsFar);

	// v5.1 Cloud / RT / DS / NPC / Traffic / Fog / Nanite
	UFUNCTION(BlueprintCallable) void ApplyCloudTierLighting(EGZCloudThickness Tier);
	UFUNCTION(BlueprintCallable) void ApplyRayTracingConfig();
	UFUNCTION(BlueprintCallable) void ApplyDirectStorageConfig();
	UFUNCTION(BlueprintCallable) void ApplyFogSpatialization(float Elevation);
	UFUNCTION(BlueprintCallable) void ApplyWeatherPreTransition(float DeltaSeconds);
	UFUNCTION(BlueprintCallable) void ApplyNaniteSeamFix();
	UFUNCTION(BlueprintCallable) void ApplyTrafficBehavior(EGZTrafficBehavior Behavior);
	UFUNCTION(BlueprintCallable) void ApplyDLSSFSRConfig();

	// v5.0 Road wetness
	UFUNCTION(BlueprintPure) EGZRoadWetState GetRoadWetState() const { return RoadWetState; }
	UFUNCTION(BlueprintPure) float GetRoadWetness() const { return RoadWetness; }
	UFUNCTION(BlueprintCallable) void SetRoadWetnessTarget(float Target);

	// v5.0 Wind
	UFUNCTION(BlueprintPure) float GetWindStrength() const { return WindStrength; }
	UFUNCTION(BlueprintCallable) void SetWindStrength(float Strength);

protected:
	void UpdateDayNightCycle(float DeltaSeconds);
	void UpdateWeatherTransition(float DeltaSeconds);
	void UpdateLightingFromZone(EGZLightingZone Zone);
	void ApplyAutoQualityPreset();
	void ApplyLumenColorBleed();
	float GetWeatherTransitionTime(EGZWeatherType From, EGZWeatherType To) const;
	int32 GetLightingZoneSampleCount(EGZLightingZone Zone) const;
	void ApplyCausalitySchedule(float Hour, EGZWeatherType Weather);
	void ApplyColorBleedCausality();
	void ApplyCellLightingConsistency();
	void ApplyCellBlendRegion();
	void ApplyWeatherLayeredLighting(float Hour, EGZWeatherType Weather);
	void ApplyFogLightingParams(float Hour);
	void ApplyVegetationBleedParams(float SunAzimuth);
	void ApplyWallBleedParams(float SunAzimuth, float SunElevation);
	void ApplyRoadBleedParams();
	void ApplyTSRDistanceWeights(float CameraDistance);
	void ApplyTSRTextureSharpening(float AmbientLightLevel);
	void ApplyGlassRefractionParams();
	void ApplyRefinedFresnelParams();
	void UpdateRoadWetness(float DeltaSeconds);
	void UpdateVegetationWind(float DeltaSeconds);
	FLinearColor KelvinToRGB(float Kelvin) const;
	bool IsHourInRange(float Hour, float RangeStart, float RangeEnd) const;
	float CosineBlendWeight(float T) const;
	float QuarticBlendWeight(float T) const;

	// Legacy
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FDayNightCycleSettings DayNight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EGZWeatherType CurrentWeather = EGZWeatherType::Clear;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EGZWeatherType TargetWeather = EGZWeatherType::Clear;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float WeatherTransitionProgress = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float WeatherTransitionDuration = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EGZLightingZone CurrentLightingZone = EGZLightingZone::OutdoorStreet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FAutoQualitySettings QualitySettings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FLumenColorBleedSettings LumenColorBleed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FAdaptiveResolutionSettings AdaptiveResolution;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FDayNightCausalityEntry> CausalitySchedule;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FColorBleedCausality ColorBleedCausality;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FCellLightingConsistency CellLightingConsistency;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FLinearColor NeutralGrayBase = FLinearColor(0.5f, 0.5f, 0.5f);

	// v5.0 Refined structures
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FBleedDecayCurve BleedDecay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVegetationBleedParams VegetationBleed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FWallBleedParams WallBleed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FRoadBleedParams RoadBleed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FCellBlendRegion CellBlend;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FWeatherLightingLayers WeatherLayers;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FFogLightingParams FogLighting;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FWeatherTransitionPhases WeatherPhases;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FRoadWetnessParams RoadWetnessParams;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVegetationWindParams WindParams;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FTSRDistanceWeight> TSRDistanceWeights;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FTSRTextureSharpening TSRSharpening;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FMultiLayerGlassParams GlassLayerParams;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FRefinedFresnelParams FresnelParams;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGlassColorNeutrality GlassNeutrality;

	// v5.0 State
	UPROPERTY() EGZRoadWetState RoadWetState = EGZRoadWetState::Dry;
	UPROPERTY() float RoadWetness = 0.0f;
	UPROPERTY() float RoadWetnessTarget = 0.0f;
	UPROPERTY() float WindStrength = 0.0f;
	UPROPERTY() float WindGustTimer = 0.0f;
	UPROPERTY() float WeatherPhaseTimer = 0.0f;
	UPROPERTY() int32 WeatherPhase = 0;
	UPROPERTY() float CloudCoverThickness = 0.0f;

	// v5.1 New configs
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FCloudTierLighting> CloudTiers;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FRayTracingConfig RayTracingConfig;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FDirectStorageConfig DirectStorageConfig;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FNPC24HLifecycle NPCLifecycle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FTrafficAIExpanded> TrafficBehaviors;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FFogSpatialization FogSpatialization;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FWeatherPreTransition WeatherPreTransition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FNaniteSeamFix NaniteSeamFix;

	UPROPERTY() EAppleSiliconChip DetectedChip = EAppleSiliconChip::Unknown;
	UPROPERTY() class ADirectionalLight* SunLight = nullptr;
	UPROPERTY() class ASkyLight* SkyLightActor = nullptr;
	UPROPERTY() class UDirectionalLightComponent* SunComponent = nullptr;
	UPROPERTY() class USkyLightComponent* SkyComponent = nullptr;

	float CurrentAdaptiveScreenPercentage = 100.0f;
	float AdaptiveTransitionTimer = 0.0f;
	float AdaptiveTransitionTarget = 100.0f;
	float AdaptiveTransitionFrom = 100.0f;
};
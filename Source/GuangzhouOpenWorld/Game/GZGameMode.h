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
enum class EGZRoadSurfaceType : uint8
{
	Asphalt		UMETA(DisplayName = "Asphalt"),
	Cement		UMETA(DisplayName = "Cement"),
	Brick		UMETA(DisplayName = "Brick"),
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

// ============================================================================
// v8.0 City Districts & Detailed Weather (Moon / GTA6 / Watch Dogs 2 / Spider-Man 2)
// ============================================================================
UENUM(BlueprintType)
enum class EGZCityDistrict : uint8
{
	TianheCBD		UMETA(DisplayName = "Tianhe CBD"),
	YuexiuOldTown	UMETA(DisplayName = "Yuexiu Old Town"),
	HaizhuWaterfront UMETA(DisplayName = "Haizhu Waterfront"),
	PanyuSuburb		UMETA(DisplayName = "Panyu Suburb"),
};

UENUM(BlueprintType)
enum class EGZWeatherStateDetailed : uint8
{
	Overcast	UMETA(DisplayName = "Overcast"),
	LightRain	UMETA(DisplayName = "Light Rain"),
	ModerateRain UMETA(DisplayName = "Moderate Rain"),
	HeavyRain	UMETA(DisplayName = "Heavy Rain"),
	DenseFog	UMETA(DisplayName = "Dense Fog"),
	StrongWind	UMETA(DisplayName = "Strong Wind"),
};

UENUM(BlueprintType)
enum class EGZCharacterRole : uint8
{
	DeliveryDriver	UMETA(DisplayName = "Delivery Driver"),
	UrbanExplorer	UMETA(DisplayName = "Urban Explorer"),
	PrivateDetective UMETA(DisplayName = "Private Detective"),
};

UENUM(BlueprintType)
enum class EGZAnomalyType : uint8
{
	LightFlicker	UMETA(DisplayName = "Light Flicker"),
	FogPocket		UMETA(DisplayName = "Fog Pocket"),
	ShadowFigure	UMETA(DisplayName = "Shadow Figure"),
	ReflectionGlitch UMETA(DisplayName = "Reflection Glitch"),
};

UENUM(BlueprintType)
enum class EGZCityEventType : uint8
{
	CommercialDispute	UMETA(DisplayName = "Commercial Dispute"),
	NeighborInteraction	UMETA(DisplayName = "Neighbor Interaction"),
	HelpRequest			UMETA(DisplayName = "Help Request"),
	RandomConflict		UMETA(DisplayName = "Random Conflict"),
	StreetPerformance	UMETA(DisplayName = "Street Performance"),
};

UENUM(BlueprintType)
enum class EGZVehicleModPart : uint8
{
	Suspension	UMETA(DisplayName = "Suspension"),
	Tires		UMETA(DisplayName = "Tires"),
	Engine		UMETA(DisplayName = "Engine"),
	BodyKit		UMETA(DisplayName = "Body Kit"),
};

UENUM(BlueprintType)
enum class EGZNPCBehaviorState : uint8
{
	Walking		UMETA(DisplayName = "Walking"),
	Standing	UMETA(DisplayName = "Standing"),
	Interacting	UMETA(DisplayName = "Interacting"),
	Reacting	UMETA(DisplayName = "Reacting"),
	Sheltering	UMETA(DisplayName = "Sheltering"),
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
	// 8 smooth resolution steps for TSR/DLSS/FSR adaptive scaling
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Step0_Ultra = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Step1_High = 95.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Step2_MediumHigh = 90.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Step3_Medium = 85.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Step4_MediumLow = 80.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Step5_Low = 75.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Step6_VeryLow = 70.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Step7_Minimum = 65.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float CBDScreenPercentage = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float StreetScreenPercentage = 90.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float FarScreenPercentage = 75.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float VehicleHighSpeedScreenPercentage = 95.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float VehicleHighSpeedThreshold = 60.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float TransitionTime = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bUseEightSteps = true;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float RoadWear = 0.0f;                // 0.0=new, 1.0=fully worn
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float RoadWearThreshold = 0.3f;       // 0-0.3 new, 0.3-1.0 worn
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
UENUM(BlueprintType)
enum class EGZCellBlendCurve : uint8
{
	Linear              UMETA(DisplayName = "Linear"),
	Cosine              UMETA(DisplayName = "Cosine"),
	Quartic             UMETA(DisplayName = "Quartic"),
	QuarticCosine       UMETA(DisplayName = "Quartic Cosine (UE5.8)")
};

USTRUCT(BlueprintType)
struct FCellBlendRegion
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 BlendWidthCells = 2;               // 2 full cells
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BlendWidthMeters = 256.0f;          // 2×128m
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EGZCellBlendCurve BlendCurve = EGZCellBlendCurve::QuarticCosine; // UE5.8 quartic-cosine
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bUseCosineCurve = true;              // Legacy flag
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SatNearNoDecay = 4000.0f;     // 0-40m no decay
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SatMidDecay = 8000.0f;        // 40-80m decay to 85%
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float FarBrightnessFloor = 0.85f;   // 80m brightness floor
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ShadowBlurMultiplier = 2.0f;  // Shadows 2x softer
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ShadowLengthReduction = 0.15f; // 15% shorter
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float LowElevationFogBoost = 0.30f; // Low-lying fog +30%
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float HighElevationReduction = 0.20f; // High elevation fog -20%
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float CoolColorDecayFaster = 1.2f;  // Cool colors decay 20% faster
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float WarmColorRetention = 0.85f;   // Warm colors retained +15%
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

	// v5.2 Material-specific drying multipliers (subtask 47)
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float AsphaltDryScale = 1.00f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float CementDryScale = 1.20f;          // cement dries 20% slower
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BrickDryScale = 0.85f;           // brick dries 15% faster (porous)

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float WetReflectivityBoost = 0.22f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float WetRoughnessReduction = 0.16f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float PuddleReflectivityExtra = 0.20f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float PuddleEvapTimeScale = 0.70f;      // 30% faster than road

	// v5.2 Puddle generation & ripple (subtask 49-51)
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float LowElevationThreshold = -200.0f;  // cm below road level
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float PuddleMaxDepth = 15.0f;           // cm
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float PuddleEvaporationRate = 0.30f;    // cm/s at full dry
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float RippleSlopeFactor = 0.50f;        // steeper slope -> stronger ripple drift
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float RippleWindFactor = 0.40f;         // wind -> larger ripples

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float View90Reflect = 0.70f;      // v7.1 cap at 90° to retain ~30% rear visibility
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxColorShiftKelvin = 30.0f;  // v7.1 global shift <=30K
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ShadowVariationAmplitude = 0.08f; // ±0.08 random shadow variation
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float CloudFlowPeriodMin = 15.0f;      // 15-45s random cloud flow
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float CloudFlowPeriodMax = 45.0f;
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
	// Distance-tiered ray tracing sample counts (0-50m=32, 50-200m=16, 200m+=8)
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 RTNearSamples = 32;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 RTMidSamples = 16;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 RTFarSamples = 8;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float RTNearDistance = 5000.0f;  // 50m
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float RTMidDistance = 20000.0f; // 200m

	// Ray-traced contact shadows with tiered samples
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bEnableContactShadows = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 ContactShadowSamples = 16;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 RTShadowNearSamples = 32;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 RTShadowMidSamples = 16;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 RTShadowFarSamples = 8;

	// Path-traced reflections with neon light boost
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bEnablePathTracingReflections = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 NeonLightSamples = 64;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 ReflectionNearSamples = 32;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 ReflectionMidSamples = 16;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 ReflectionFarSamples = 8;
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
struct FWeatherTransitionStages
{
	GENERATED_BODY()
	// 2.8s total: 0-1.2s direct sun, 1.2-2.0s color temp, 2.0-2.8s contrast/shadow softening
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float DirectSunEnd = 1.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ColorTempEnd = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float FinalEnd = 2.8f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float DirectSunIntensityDrop = 1.0f;  // clear->rain drops sun intensity by 100%
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float RainColorTempK = 7000.0f;       // rainy overcast color temp
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ContrastDrop = 0.4f;            // rain reduces contrast 40%
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ShadowBlurBoost = 1.0f;         // rain shadows 2x softer
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ShadowLengthScale = 0.85f;      // rain shadows 15% shorter
};

USTRUCT(BlueprintType)
struct FNaniteSeamFix
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float VertexInterpolationDistance = 0.01f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxSeamGap = 0.01f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bEnableSeamBlend = true;
};

// ============================================================================
// v5.2 Loading priority tiers (subtask 59)
// ============================================================================
USTRUCT(BlueprintType)
struct FLoadingPriorityTier
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString TierName = TEXT("Normal");
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float PriorityWeight = 1.0f;          // higher = load first
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float CullDistanceScale = 1.0f;       // streaming distance multiplier
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 MinLOD = 0;
};

// ============================================================================
// v5.2 Physics-driven wind config (subtask 52-57)
// Replaces any preset keyframe animation with real-time physics forces
// ============================================================================
USTRUCT(BlueprintType)
struct FPhysicsWindConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bEnablePhysicsWind = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxSignSwingAngle = 15.0f;       // degrees, prevents clipping
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxTreeBranchAngle = 8.0f;       // degrees
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxLeafAngle = 25.0f;            // degrees
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float PhysicsForceMultiplier = 100.0f; // scales wind force on bodies
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Damping = 0.92f;                 // angular velocity decay
};

// ============================================================================
// v8.0 WorldPartition District Profiles (Moon / GTA6)
// ============================================================================
USTRUCT(BlueprintType)
struct FDistrictRoadProfile
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float RoadWear = 0.0f;              // 0=new, 1=worn
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float RoughnessScale = 1.0f;        // roughness multiplier
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float PuddleEvaporationScale = 1.0f;// district evaporation speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float FrictionDry = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float FrictionWet = 0.65f;
};

USTRUCT(BlueprintType)
struct FDistrictVegetationProfile
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EGZVegetationType PrimaryVeg = EGZVegetationType::Broadleaf;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float WindResponseScale = 1.0f;     // per-district wind amplitude
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float DensityScale = 1.0f;          // vegetation density
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ColorBleedBoost = 0.0f;       // district-specific bleed offset
};

USTRUCT(BlueprintType)
struct FDistrictProfile
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EGZCityDistrict District = EGZCityDistrict::TianheCBD;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FDistrictRoadProfile RoadProfile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FDistrictVegetationProfile VegetationProfile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float NeonIntensityScale = 1.0f;    // CBD high, suburb low
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float PedestrianSpeedScale = 1.0f;  // CBD fast, old-town slow
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float LandmarkVisibilityDistance = 100000.0f; // 1000m global landmark
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FLinearColor FarCoolTint = FLinearColor(0.75f, 0.85f, 1.0f, 1.0f);
};

// ============================================================================
// v8.0 Ray Tracing / Rendering Refinement (GTA6 / Spider-Man 2 / Cyberpunk 2077)
// ============================================================================
USTRUCT(BlueprintType)
struct FNeonLightParams
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float DayIntensityScale = 0.15f;    // weak bleed by day
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float NightIntensityScale = 1.0f;   // strong bleed at night
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float CBDIntensityScale = 1.3f;     // CBD stronger
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float OldTownIntensityScale = 0.6f; // old town weaker
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BleedDecayExponent = 2.2f;    // physical falloff
};

USTRUCT(BlueprintType)
struct FWaterReflectionParams
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float DeepWaterSampleDistance = 200000.0f; // 2000m reflection range
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float PuddleSampleDistance = 30000.0f;     // 300m puddle range
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float DeepWaterDistortion = 0.08f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float PuddleDistortion = 0.25f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float VehicleUnderbodyVisibility = 0.7f;   // puddle maps undercarriage
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ShallowPuddleClarity = 0.85f;        // shallow puddle clearer
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float DeepPuddleClarity = 0.45f;           // deep puddle murkier
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float VehicleDisturbanceRecovery = 1.5f;   // ripple settle speed
};

USTRUCT(BlueprintType)
struct FCharacterHairSkinParams
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 HairSamplesPerStrand = 4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float VolumetricScatteringIntensity = 0.35f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SkinSubsurfaceScale = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float EnvironmentLightBlend = 1.0f; // match scene lighting
};

USTRUCT(BlueprintType)
struct FMetallicMaterialRTParams
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bEnableMetallicRTSpecular = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float FresnelRangeScale = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float EnvironmentLightResponse = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float FarDistanceFadeStart = 50000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float FarDistanceFadeEnd = 150000.0f;
};

USTRUCT(BlueprintType)
struct FGlassOverlapBlendParams
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float OverlapBlendRadius = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float OverlapColorDamping = 0.6f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 MaxOverlappingFacades = 4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bResolveColorConflict = true;
};

// ============================================================================
// v8.0 Dynamic Weather / Lighting Parameters
// ============================================================================
USTRUCT(BlueprintType)
struct FAtmosphericColorDecay
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float NearDistance = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float FarDistance = 100000.0f;       // 1000m
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float CoolBlueShift = 0.25f;        // far tint strength
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float WarmRetention = 0.85f;        // near warm retention
};

USTRUCT(BlueprintType)
struct FCloudShadowParams
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ShadowIntensity = 0.35f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float CloudMoveSpeedBase = 2.0f;     // m/s
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float WindSpeedMultiplier = 1.5f;    // speed scales with wind
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BlendTransitionTime = 3.0f;    // soft patch movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float LowElevationIntensityBoost = 0.15f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float CloudThicknessToShadowScale = 1.0f;
};

USTRUCT(BlueprintType)
struct FDetailedWeatherStateParams
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SunIntensityScale = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SkyIntensityScale = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ColorTempKelvin = 6500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float FogDensityScale = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float FogShadowBlurScale = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float FogShadowLengthScale = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float WindStrengthScale = 1.0f;
};

// ============================================================================
// v8.0 Gameplay Systems Parameters (GTA6 / Beyond / Infinite)
// ============================================================================
USTRUCT(BlueprintType)
struct FDualCharacterData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString CharacterName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 SaveSlotIndex = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EGZCharacterRole Role = EGZCharacterRole::UrbanExplorer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector LastLocation = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bIsActive = false;
};

USTRUCT(BlueprintType)
struct FNPCBehaviorProfile
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EGZCityDistrict District = EGZCityDistrict::TianheCBD;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BaseWalkSpeed = 200.0f;       // cm/s
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float StopChance = 0.05f;           // chance per tick to stop
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float InteractionChance = 0.10f;    // chance to interact with other NPC
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ShelterChance = 0.30f;        // chance to seek shelter in rain
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float VisionRange = 5000.0f;        // 50m
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float HearingRange = 8000.0f;       // 80m
};

USTRUCT(BlueprintType)
struct FCityEventConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EGZCityEventType EventType = EGZCityEventType::CommercialDispute;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EGZCityDistrict PreferredDistrict = EGZCityDistrict::TianheCBD;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SpawnChance = 0.02f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MinCooldown = 60.0f;          // seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxRadius = 30000.0f;         // 300m
};

USTRUCT(BlueprintType)
struct FVehicleModConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EGZVehicleModPart Part = EGZVehicleModPart::Suspension;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SuspensionRollScale = 1.0f;   // body roll multiplier
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float TireDryFriction = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float TireWetFriction = 0.7f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float EnginePowerScale = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BodyDragScale = 1.0f;
};

USTRUCT(BlueprintType)
struct FShopInteractionConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float OpenHour = 8.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float CloseHour = 22.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float RainCloseChance = 0.30f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float InteractionRadius = 250.0f;   // cm
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bIndoorLightingSwitch = true;
};

USTRUCT(BlueprintType)
struct FAnomalyPointConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EGZAnomalyType Type = EGZAnomalyType::LightFlicker;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EGZCityDistrict District = EGZCityDistrict::TianheCBD;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector Location = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float TriggerRadius = 1000.0f;      // 10m
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Duration = 30.0f;             // seconds
};

USTRUCT(BlueprintType)
struct FFloodParams
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float RainToWaterRate = 0.5f;       // cm water per rain intensity unit
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxWaterHeight = 50.0f;       // cm
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float EvaporationRate = 0.05f;      // cm/s
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float VehicleSpeedPenalty = 0.4f;   // 40% slower
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float NPCRerouteThreshold = 20.0f;  // cm
};

USTRUCT(BlueprintType)
struct FProfessionConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EGZCharacterRole Role = EGZCharacterRole::UrbanExplorer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ShopDiscount = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float EventRevealChance = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float NPCFriendlinessOffset = 0.0f;
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
	UFUNCTION(BlueprintCallable) void ApplyLoadingPriorities();
	UFUNCTION(BlueprintCallable) void ApplyPhysicsWindConfig();

	// v5.0 Road wetness
	UFUNCTION(BlueprintPure) EGZRoadWetState GetRoadWetState() const { return RoadWetState; }
	UFUNCTION(BlueprintPure) float GetRoadWetness() const { return RoadWetness; }
	UFUNCTION(BlueprintCallable) void SetRoadWetnessTarget(float Target);
	UFUNCTION(BlueprintCallable) void SetRoadSurfaceType(EGZRoadSurfaceType SurfaceType);
	UFUNCTION(BlueprintPure) EGZRoadSurfaceType GetRoadSurfaceType() const { return CurrentRoadSurfaceType; }

	// v5.0 Wind
	UFUNCTION(BlueprintPure) float GetWindStrength() const { return WindStrength; }
	UFUNCTION(BlueprintPure) float GetEffectiveWindStrength() const;
	UFUNCTION(BlueprintPure) float GetWindGustTimer() const { return WindGustTimer; }
	UFUNCTION(BlueprintCallable) void SetWindStrength(float Strength);
	UFUNCTION(BlueprintPure) const FVegetationWindParams& GetWindParams() const { return WindParams; }
	UFUNCTION(BlueprintPure) const FPhysicsWindConfig& GetPhysicsWindConfig() const { return PhysicsWindConfig; }

	// v8.0 District / Rendering / Weather / Gameplay public accessors
	UFUNCTION(BlueprintCallable) void SetDetailedWeather(EGZWeatherStateDetailed NewWeather);
	UFUNCTION(BlueprintPure) EGZWeatherStateDetailed GetDetailedWeather() const { return CurrentDetailedWeather; }
	UFUNCTION(BlueprintCallable) void SetCurrentDistrict(EGZCityDistrict District);
	UFUNCTION(BlueprintPure) EGZCityDistrict GetCurrentDistrict() const { return CurrentDistrict; }
	UFUNCTION(BlueprintCallable) void ApplyDistrictProfile(EGZCityDistrict District);
	UFUNCTION(BlueprintCallable) void ApplyNeonLightParams();
	UFUNCTION(BlueprintCallable) void ApplyWaterReflectionParams();
	UFUNCTION(BlueprintCallable) void ApplyCharacterHairSkinParams();
	UFUNCTION(BlueprintCallable) void ApplyAtmosphericColorDecay();
	UFUNCTION(BlueprintCallable) void ApplyCloudShadowParams();
	UFUNCTION(BlueprintCallable) void ApplyFloodParams();
	UFUNCTION(BlueprintCallable) void ApplyMetallicMaterialRTParams();
	UFUNCTION(BlueprintCallable) void ApplyGlassOverlapBlendParams();
	UFUNCTION(BlueprintCallable) void ApplyDetailedWeatherStateParams(EGZWeatherStateDetailed State);

	UFUNCTION(BlueprintPure) float GetCurrentFloodHeight() const { return CurrentFloodHeight; }
	UFUNCTION(BlueprintPure) bool IsIndoor() const { return bIsIndoor; }
	UFUNCTION(BlueprintCallable) void SetIndoor(bool bIndoor);

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
	void ApplyTSRDistanceWeights(float CameraDistance, float VehicleSpeedKmh = 0.0f);
	void ApplyTSRTextureSharpening(float AmbientLightLevel);
	void ApplyGlassRefractionParams();
	void ApplyRefinedFresnelParams();
	void UpdateRoadWetness(float DeltaSeconds);
	void UpdateVegetationWind(float DeltaSeconds);
	void UpdatePuddles(float DeltaSeconds);
	float ComputeRoadSurfaceDryScale() const;
	float ComputeSunFacingDryFactor() const;
	FLinearColor KelvinToRGB(float Kelvin) const;
	bool IsHourInRange(float Hour, float RangeStart, float RangeEnd) const;
	float CosineBlendWeight(float T) const;
	float QuarticBlendWeight(float T) const;
	float QuarticCosineBlendWeight(float T) const;

	// v8.0 district / weather / gameplay update helpers
	void UpdateDistrictProfiles();
	void UpdateFloodState(float DeltaSeconds);
	void UpdateCloudShadows(float DeltaSeconds);
	void UpdateDetailedWeatherTransition(float DeltaSeconds);
	float GetDetailedWeatherIntensity() const;

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
	UPROPERTY() EGZRoadSurfaceType CurrentRoadSurfaceType = EGZRoadSurfaceType::Asphalt;
	UPROPERTY() float RoadWetness = 0.0f;
	UPROPERTY() float RoadWetnessTarget = 0.0f;
	UPROPERTY() float WindStrength = 0.0f;
	UPROPERTY() float WindGustTimer = 0.0f;
	UPROPERTY() float WeatherPhaseTimer = 0.0f;
	UPROPERTY() int32 WeatherPhase = 0;
	UPROPERTY() float CloudCoverThickness = 0.0f;

	// v5.2 Puddle & ripple state (subtask 49-51)
	UPROPERTY() float PuddleAmount = 0.0f;          // 0-1 saturation of low areas
	UPROPERTY() float PuddleDepth = 0.0f;           // current depth cm
	UPROPERTY() float RippleIntensity = 0.0f;       // 0-1 ripple strength
	UPROPERTY() FVector2D RippleDirection = FVector2D::ZeroVector;

	// v5.1 New configs
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FCloudTierLighting> CloudTiers;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FRayTracingConfig RayTracingConfig;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FDirectStorageConfig DirectStorageConfig;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FNPC24HLifecycle NPCLifecycle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FTrafficAIExpanded> TrafficBehaviors;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FFogSpatialization FogSpatialization;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FWeatherPreTransition WeatherPreTransition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FWeatherTransitionStages WeatherStages;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FNaniteSeamFix NaniteSeamFix;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FPhysicsWindConfig PhysicsWindConfig;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FLoadingPriorityTier> LoadingPriorityTiers;

	// v8.0 District / Rendering / Weather / Gameplay config structures
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FDistrictProfile> DistrictProfiles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FNeonLightParams NeonParams;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FWaterReflectionParams WaterReflection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FCharacterHairSkinParams CharacterHairSkin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FAtmosphericColorDecay AtmosphericDecay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FCloudShadowParams CloudShadow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FMetallicMaterialRTParams MetallicRTParams;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FGlassOverlapBlendParams GlassOverlapParams;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<EGZWeatherStateDetailed, FDetailedWeatherStateParams> DetailedWeatherParams;

	// v8.0 Gameplay system config structures
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FDualCharacterData> DualCharacterData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FNPCBehaviorProfile> NPCBehaviorProfiles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FCityEventConfig> CityEventConfigs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FVehicleModConfig> VehicleModConfigs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FShopInteractionConfig> ShopConfigs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FAnomalyPointConfig> AnomalyConfigs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FFloodParams FloodParams;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FProfessionConfig> ProfessionConfigs;

	UPROPERTY() EAppleSiliconChip DetectedChip = EAppleSiliconChip::Unknown;
	UPROPERTY() class ADirectionalLight* SunLight = nullptr;
	UPROPERTY() class ASkyLight* SkyLightActor = nullptr;
	UPROPERTY() class UDirectionalLightComponent* SunComponent = nullptr;
	UPROPERTY() class USkyLightComponent* SkyComponent = nullptr;

	float CurrentAdaptiveScreenPercentage = 100.0f;
	float AdaptiveTransitionTimer = 0.0f;
	float AdaptiveTransitionTarget = 100.0f;
	float AdaptiveTransitionFrom = 100.0f;

	// v8.0 runtime state
	UPROPERTY() EGZCityDistrict CurrentDistrict = EGZCityDistrict::TianheCBD;
	UPROPERTY() EGZWeatherStateDetailed CurrentDetailedWeather = EGZWeatherStateDetailed::Overcast;
	UPROPERTY() EGZWeatherStateDetailed TargetDetailedWeather = EGZWeatherStateDetailed::Overcast;
	UPROPERTY() float DetailedWeatherTransitionProgress = 1.0f;
	UPROPERTY() float CurrentFloodHeight = 0.0f;
	UPROPERTY() float CloudShadowOffset = 0.0f;
	UPROPERTY() bool bIsIndoor = false;
	UPROPERTY() int32 ActiveCharacterIndex = 0;
};
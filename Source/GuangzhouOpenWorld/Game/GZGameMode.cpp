#include "GZGameMode.h"
#include "GZPlayerController.h"
#include "GZCharacter.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformMisc.h"

AGZGameMode::AGZGameMode()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 0.5f;
    DefaultPawnClass = AGZCharacter::StaticClass();
    PlayerControllerClass = AGZPlayerController::StaticClass();
}

void AGZGameMode::BeginPlay()
{
    Super::BeginPlay();
    // Detect Apple Silicon chip and apply chip-specific settings
    DetectedChip = DetectAppleSiliconChip();
    ApplyChipSpecificSettings();
    UE_LOG(LogTemp, Log, TEXT("GTA-广州 GameMode BeginPlay - Guangzhou Open World | Chip: %d"), (int32)DetectedChip);
    WeatherTimer = WeatherChangeInterval;
}

EAppleSiliconChip AGZGameMode::DetectAppleSiliconChip()
{
    // Detect Apple Silicon chip via sysctl or CPU info
    FString CPUInfo = FPlatformMisc::GetCPUBrand();
    if (CPUInfo.Contains(TEXT("M3"))) return EAppleSiliconChip::M3;
    if (CPUInfo.Contains(TEXT("M2"))) return EAppleSiliconChip::M2;
    if (CPUInfo.Contains(TEXT("M1"))) return EAppleSiliconChip::M1;
    // Fallback: check performance core count
    int32 CoreCount = FPlatformMisc::NumberOfCoresIncludingHyperthreads();
    if (CoreCount >= 14) return EAppleSiliconChip::M3; // M3 Max/Ultra
    if (CoreCount >= 8) return EAppleSiliconChip::M2;  // M2 family
    return EAppleSiliconChip::M1; // M1 family
}

void AGZGameMode::ApplyChipSpecificSettings()
{
    bool bIsM2OrBetter = (DetectedChip == EAppleSiliconChip::M2 || DetectedChip == EAppleSiliconChip::M3);
    bool bIsM1 = (DetectedChip == EAppleSiliconChip::M1);

    auto SetCVar = [](const TCHAR* Name, float Value) {
        if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(Name))
            CVar->Set(Value);
    };

    if (bIsM1)
    {
        // M1: Lumen software ray tracing, reduced sampling
        SetCVar(TEXT("r.Lumen.ScreenProbeGather.NumAdaptiveProbes"), 16);
        SetCVar(TEXT("r.Lumen.ScreenProbeGather.RadianceCache.NumProbesToTraceBudget"), 150);
        SetCVar(TEXT("r.Lumen.ScreenProbeGather.MaxRayIntensity"), 20);
        SetCVar(TEXT("r.Lumen.MaxBounces"), 3);
        // M1: Nanite moderate precision
        SetCVar(TEXT("r.Nanite.MaxPixelsPerEdge"), 2);
        SetCVar(TEXT("r.Nanite.MaxNodes"), 1024);
        SetCVar(TEXT("r.Nanite.MaxVisibleClusters"), 262144);
        SetCVar(TEXT("r.Nanite.StreamingPool"), 1024);
        // M1: VSM reduced
        SetCVar(TEXT("r.Shadow.Virtual.Enable"), 0);
        SetCVar(TEXT("r.Shadow.Virtual.SMRT.RayCountDirectional"), 8);
        // M1: TSR quality mode
        SetCVar(TEXT("r.TSR.History.ScreenPercentage"), 150);
        SetCVar(TEXT("r.TSR.History.SampleCount"), 8);
        // M1: Particle limits
        SetCVar(TEXT("fx.Niagara.MaxGPUParticles"), 500000);
        SetCVar(TEXT("fx.Niagara.MaxCPUParticles"), 50000);
        SetCVar(TEXT("fx.Niagara.QualityLevel"), 3);
        // M1: Reduced post-processing
        SetCVar(TEXT("r.BloomQuality"), 4);
        SetCVar(TEXT("r.RefractionQuality"), 2);
        SetCVar(TEXT("r.SSR.Quality"), 3);
        // M1: Reduced volumetric
        SetCVar(TEXT("r.VolumetricFog.GridPixelSize"), 8);
        SetCVar(TEXT("r.VolumetricFog.GridSizeZ"), 96);
        // M1: Reduced streaming pool
        SetCVar(TEXT("r.Streaming.PoolSize"), 2048);
        UE_LOG(LogTemp, Log, TEXT("Apple Silicon M1 settings applied: Lumen SW, Nanite moderate, VSM off, particles limited"));
    }
    else if (bIsM2OrBetter)
    {
        // M2/M3: Full cinematic, max sampling
        SetCVar(TEXT("r.Lumen.ScreenProbeGather.NumAdaptiveProbes"), 32);
        SetCVar(TEXT("r.Lumen.ScreenProbeGather.RadianceCache.NumProbesToTraceBudget"), 300);
        SetCVar(TEXT("r.Lumen.ScreenProbeGather.MaxRayIntensity"), 40);
        SetCVar(TEXT("r.Lumen.MaxBounces"), 4);
        // M2/M3: Nanite highest precision
        SetCVar(TEXT("r.Nanite.MaxPixelsPerEdge"), 1);
        SetCVar(TEXT("r.Nanite.MaxNodes"), 2048);
        SetCVar(TEXT("r.Nanite.MaxVisibleClusters"), 524288);
        SetCVar(TEXT("r.Nanite.StreamingPool"), 2048);
        // M2/M3: VSM full quality
        SetCVar(TEXT("r.Shadow.Virtual.Enable"), 1);
        SetCVar(TEXT("r.Shadow.Virtual.SMRT.RayCountDirectional"), 16);
        // M2/M3: TSR max quality
        SetCVar(TEXT("r.TSR.History.ScreenPercentage"), 200);
        SetCVar(TEXT("r.TSR.History.SampleCount"), 16);
        // M2/M3: Unlimited particles
        SetCVar(TEXT("fx.Niagara.MaxGPUParticles"), 5000000);
        SetCVar(TEXT("fx.Niagara.MaxCPUParticles"), 500000);
        SetCVar(TEXT("fx.Niagara.QualityLevel"), 4);
        // M2/M3: Full post-processing
        SetCVar(TEXT("r.BloomQuality"), 5);
        SetCVar(TEXT("r.RefractionQuality"), 3);
        SetCVar(TEXT("r.SSR.Quality"), 4);
        // M2/M3: Full volumetric
        SetCVar(TEXT("r.VolumetricFog.GridPixelSize"), 4);
        SetCVar(TEXT("r.VolumetricFog.GridSizeZ"), 128);
        // M2/M3: Full streaming pool
        SetCVar(TEXT("r.Streaming.PoolSize"), 4096);
        UE_LOG(LogTemp, Log, TEXT("Apple Silicon M2/M3 settings applied: Lumen cinematic, Nanite max, VSM full, particles unlimited"));
    }
}

void AGZGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // Day/night cycle
    TimeOfDay += DayCycleSpeed * DeltaTime;
    if (TimeOfDay > 1.0f) TimeOfDay -= 1.0f;
    // Weather cycling
    WeatherTimer -= DeltaTime;
    if (WeatherTimer <= 0.0f) { CycleWeather(); WeatherTimer = WeatherChangeInterval; }
    // Smooth weather blend
    WeatherBlend = FMath::FInterpTo(WeatherBlend, CurrentWeather == 2 ? 1.0f : 0.0f, DeltaTime, 0.5f);
}

void AGZGameMode::CycleWeather()
{
    // Weighted random: mostly clear, occasional rain
    float Rand = FMath::FRand();
    if (Rand < 0.55f) CurrentWeather = 0; // Clear
    else if (Rand < 0.75f) CurrentWeather = 1; // Cloudy
    else if (Rand < 0.95f) CurrentWeather = 2; // Rain
    else CurrentWeather = 3; // Fog
    UE_LOG(LogTemp, Log, TEXT("Weather changed to: %d"), CurrentWeather);
}

void AGZGameMode::UpdateAutoQuality(float CurrentFPS)
{
    FPSHistory.Add(CurrentFPS);
    if (FPSHistory.Num() > 60) FPSHistory.RemoveAt(0);
    if (FPSHistory.Num() < 30) return;
    float AvgFPS = 0.0f;
    for (float F : FPSHistory) AvgFPS += F;
    AvgFPS /= FPSHistory.Num();
    float MinRecent = FPSHistory[0];
    for (int32 i = 1; i < FPSHistory.Num(); i++) MinRecent = FMath::Min(MinRecent, FPSHistory[i]);
    if (MinRecent < MinFPS && CurrentQualityLevel < 4) { CurrentQualityLevel++; ApplyQualitySettings(CurrentQualityLevel); }
    else if (AvgFPS > TargetFPS + 10 && CurrentQualityLevel > 0) { CurrentQualityLevel--; ApplyQualitySettings(CurrentQualityLevel); }
}

void AGZGameMode::ApplyQualitySettings(int32 Level)
{
    static const TArray<TPair<FString, TArray<float>>> Settings = {
        {TEXT("r.ScreenPercentage"), {100, 85, 70, 60, 50}},
        {TEXT("r.Lumen.ScreenProbeGather.NumAdaptiveProbes"), {32, 24, 16, 12, 8}},
        {TEXT("r.Shadow.Virtual.ResolutionLodBiasDirectional"), {-1, 0, 1, 2, 3}},
        {TEXT("r.Nanite.MaxPixelsPerEdge"), {1, 2, 3, 4, 8}},
        {TEXT("r.VolumetricFog.GridPixelSize"), {4, 8, 12, 16, 32}},
        {TEXT("r.SSR.Quality"), {4, 3, 2, 1, 0}},
        {TEXT("r.BloomQuality"), {5, 4, 3, 2, 0}},
        {TEXT("r.DepthOfFieldQuality"), {3, 2, 1, 0, 0}},
        {TEXT("r.MotionBlurQuality"), {4, 3, 2, 0, 0}},
    };
    UE_LOG(LogTemp, Log, TEXT("AutoQuality: Level %d applied"), Level);
    for (const auto& S : Settings)
    {
        if (Level < S.Value.Num())
        {
            if (IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(*S.Key))
            {
                CVar->Set(S.Value[Level]);
            }
        }
    }
}
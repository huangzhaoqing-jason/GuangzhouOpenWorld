#include "GZGameMode.h"
#include "GZPlayerController.h"
#include "GZCharacter.h"
#include "Engine/World.h"
#include "EngineUtils.h"

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
    UE_LOG(LogTemp, Log, TEXT("GTA-广州 GameMode BeginPlay - Guangzhou Open World"));
    WeatherTimer = WeatherChangeInterval;
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
    // Apply console variables based on quality level
    // Level 0=Ultra, 1=High, 2=Medium, 3=Low, 4=Potato
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
            FString Cmd = FString::Printf(TEXT("%s %f"), *S.Key, S.Value[Level]);
            GEngine->Exec(GetWorld(), *Cmd);
        }
    }
}
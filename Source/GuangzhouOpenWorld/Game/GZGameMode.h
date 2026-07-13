// GZGameMode.h - GTA-广州 Game Mode
// Controls game rules, spawning, and session management
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GZGameMode.generated.h"

UCLASS()
class GUANGZHOUOPENWORLD_API AGZGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    AGZGameMode();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // World state
    UPROPERTY(BlueprintReadOnly) float TimeOfDay = 0.5f; // 0=midnight, 0.5=noon
    UPROPERTY(BlueprintReadOnly) float WeatherBlend = 0.0f; // 0=clear, 1=storm
    UPROPERTY(BlueprintReadOnly) int32 CurrentWeather = 0; // 0=clear, 1=cloudy, 2=rain, 3=fog

    UPROPERTY(EditDefaultsOnly) float DayCycleSpeed = 0.001f;
    UPROPERTY(EditDefaultsOnly) float WeatherChangeInterval = 120.0f;

    // Auto Quality Manager
    UPROPERTY(EditDefaultsOnly) int32 TargetFPS = 55;
    UPROPERTY(EditDefaultsOnly) int32 MinFPS = 25;
    void UpdateAutoQuality(float CurrentFPS);

private:
    float WeatherTimer = 0.0f;
    TArray<float> FPSHistory;
    int32 CurrentQualityLevel = 3; // 0=Ultra, 1=High, 2=Medium, 3=Low
    void CycleWeather();
    void ApplyQualitySettings(int32 Level);
};
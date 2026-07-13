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

USTRUCT(BlueprintType)
struct FDayNightCycleSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	float TimeOfDay = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	float DayLengthSeconds = 1440.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	float SunAngle = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	float SunIntensity = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	float NightIntensity = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	FLinearColor DayColor = FLinearColor(1.0f, 0.95f, 0.8f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	FLinearColor NightColor = FLinearColor(0.1f, 0.15f, 0.4f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	FLinearColor SunsetColor = FLinearColor(1.0f, 0.5f, 0.2f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	FLinearColor SunriseColor = FLinearColor(0.9f, 0.6f, 0.3f, 1.0f);
};

USTRUCT(BlueprintType)
struct FAutoQualitySettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quality")
	float ScreenPercentage = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quality")
	int32 ShadowResolution = 2048;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quality")
	int32 VolumetricFogGridSize = 128;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quality")
	int32 SSRQuality = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quality")
	float BloomIntensity = 1.0f;
};

UCLASS()
class GUANGZHOUOPENWORLD_API AGZGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGZGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "AppleSilicon")
	EAppleSiliconChip DetectAppleSiliconChip() const;

	UFUNCTION(BlueprintCallable, Category = "AppleSilicon")
	void ApplyChipSpecificSettings();

	UFUNCTION(BlueprintCallable, Category = "Weather")
	void SetWeather(EGZWeatherType NewWeather, float TransitionTime = 5.0f);

	UFUNCTION(BlueprintPure, Category = "Weather")
	EGZWeatherType GetCurrentWeather() const { return CurrentWeather; }

	UFUNCTION(BlueprintCallable, Category = "Lighting")
	void SetLightingZone(EGZLightingZone NewZone);

	UFUNCTION(BlueprintPure, Category = "Lighting")
	EGZLightingZone GetCurrentLightingZone() const { return CurrentLightingZone; }

	UFUNCTION(BlueprintPure, Category = "DayNight")
	float GetTimeOfDay() const { return DayNight.TimeOfDay; }

	UFUNCTION(BlueprintCallable, Category = "DayNight")
	void SetTimeOfDay(float NewTime);

protected:
	void UpdateDayNightCycle(float DeltaSeconds);
	void UpdateWeatherTransition(float DeltaSeconds);
	void UpdateLightingFromZone(EGZLightingZone Zone);
	void ApplyAutoQualityPreset();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DayNight")
	FDayNightCycleSettings DayNight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	EGZWeatherType CurrentWeather = EGZWeatherType::Clear;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	EGZWeatherType TargetWeather = EGZWeatherType::Clear;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	float WeatherTransitionProgress = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	float WeatherTransitionSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighting")
	EGZLightingZone CurrentLightingZone = EGZLightingZone::OutdoorStreet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quality")
	FAutoQualitySettings QualitySettings;

	UPROPERTY()
	EAppleSiliconChip DetectedChip = EAppleSiliconChip::Unknown;

	UPROPERTY()
	class ADirectionalLight* SunLight = nullptr;

	UPROPERTY()
	class ASkyLight* SkyLightActor = nullptr;

	UPROPERTY()
	class UDirectionalLightComponent* SunComponent = nullptr;

	UPROPERTY()
	class USkyLightComponent* SkyComponent = nullptr;
};
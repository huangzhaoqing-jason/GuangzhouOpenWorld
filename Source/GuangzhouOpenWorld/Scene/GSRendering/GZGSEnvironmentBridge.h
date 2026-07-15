#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZGSEnvironmentBridge.generated.h"

USTRUCT(BlueprintType)
struct FGSGSEnvironmentParameters
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	float Opacity = 1.0f;

	UPROPERTY(BlueprintReadOnly)
	float Saturation = 1.0f;

	UPROPERTY(BlueprintReadOnly)
	float WindDisplacement = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float Wetness = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float ReflectionIntensity = 0.5f;

	UPROPERTY(BlueprintReadOnly)
	float ColorTemperature = 6500.0f;

	UPROPERTY(BlueprintReadOnly)
	float LightIntensityScale = 1.0f;
};

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZGSEnvironmentBridge : public UObject
{
	GENERATED_BODY()

public:
	UGZGSEnvironmentBridge();

	UFUNCTION(BlueprintCallable)
	void Initialize();

	UFUNCTION(BlueprintCallable)
	void SetEnvironment(float InHumidity01, float InWindSpeed, float InLightIntensity, float InRainIntensity, float InTimeOfDay);

	UFUNCTION(BlueprintPure)
	const FGSGSEnvironmentParameters& GetParameters() const { return Parameters; }

	UFUNCTION(BlueprintPure)
	float GetHumidity01() const { return Humidity01; }

	UFUNCTION(BlueprintPure)
	float GetWetness01() const { return Parameters.Wetness; }

private:
	void RecalculateParameters();

	float Humidity01 = 0.0f;
	float WindSpeed = 0.0f;
	float LightIntensity = 1.0f;
	float RainIntensity = 0.0f;
	float TimeOfDay = 12.0f;

	FGSGSEnvironmentParameters Parameters;
};

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZSoLoudReverb.generated.h"

UENUM(BlueprintType)
enum class EGZSoLoudReverbMode : uint8
{
	Indoor      UMETA(DisplayName="Indoor"),
	Outdoor     UMETA(DisplayName="Outdoor"),
	CityStreet  UMETA(DisplayName="City Street"),
	Enclosed    UMETA(DisplayName="Enclosed Building"),
};

USTRUCT(BlueprintType)
struct FZSoLoudReverbParams
{
	GENERATED_BODY()

	UPROPERTY() float RoomSize = 1.0f;
	UPROPERTY() float Damping = 0.5f;
	UPROPERTY() float WetLevel = 0.3f;
	UPROPERTY() float DecayTime = 1.5f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOLOUD_API UGZSoLoudReverb : public UObject
{
	GENERATED_BODY()

public:
	UGZSoLoudReverb();

	UFUNCTION(BlueprintCallable, Category="SoLoudReverb")
	void SetMode(EGZSoLoudReverbMode Mode);

	UFUNCTION(BlueprintCallable, Category="SoLoudReverb")
	EGZSoLoudReverbMode DetectMode(const FVector& Location, bool bIndoors, bool bEnclosed) const;

	UFUNCTION(BlueprintCallable, Category="SoLoudReverb")
	FZSoLoudReverbParams GetCurrentParams() const;

	UFUNCTION(BlueprintCallable, Category="SoLoudReverb")
	void SetWeatherWetLevelScale(float Scale);

private:
	EGZSoLoudReverbMode CurrentMode = EGZSoLoudReverbMode::Outdoor;
	float WeatherScale = 1.0f;
};

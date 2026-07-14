#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZSoLoudChipConfig.generated.h"

USTRUCT(BlueprintType)
struct FZSoLoudChipConfig
{
	GENERATED_BODY()

	UPROPERTY() int32 MaxConcurrentSounds = 64;
	UPROPERTY() int32 MaxSpatialVoices = 32;
	UPROPERTY() int32 SampleRate = 48000;
	UPROPERTY() bool bUseHardwareDSP = true;
	UPROPERTY() bool bBackgroundLowSampleRate = true;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOLOUD_API UGZSoLoudChipConfig : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="SoLoudChip")
	static FZSoLoudChipConfig GetConfigForCurrentMachine();
};

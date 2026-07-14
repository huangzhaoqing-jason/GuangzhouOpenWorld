#pragma once

#include "CoreMinimal.h"
#include "HAL/Thread.h"
#include "HAL/Runnable.h"
#include "GZJoltThreadConfig.generated.h"

UENUM(BlueprintType)
enum class EJoltChipProfile : uint8
{
	M1 UMETA(DisplayName="Apple M1"),
	M2 UMETA(DisplayName="Apple M2"),
	M3 UMETA(DisplayName="Apple M3"),
};

USTRUCT(BlueprintType)
struct FZJoltThreadConfig
{
	GENERATED_BODY()

	UPROPERTY() EJoltChipProfile ChipProfile = EJoltChipProfile::M1;
	UPROPERTY() int32 PerformanceThreads = 4;
	UPROPERTY() float UpdateFrequencyHz = 60.0f;
	UPROPERTY() float FarDistanceUpdateHz = 2.0f;
	UPROPERTY() float CPULimitThreshold = 0.8f;
	UPROPERTY() bool bFullPrecision = false;
};

class FZJoltThreadConfigProvider
{
public:
	static FZJoltThreadConfig DetectChipProfile();
	static int32 CountPerformanceThreads();
};

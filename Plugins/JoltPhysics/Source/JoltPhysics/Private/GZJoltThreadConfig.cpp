#include "GZJoltThreadConfig.h"
#include "HAL/PlatformMisc.h"

FZJoltThreadConfig FZJoltThreadConfigProvider::DetectChipProfile()
{
	FZJoltThreadConfig Config;

#if PLATFORM_MAC
	FString CPUBrand = FPlatformMisc::GetCPUBrand();
	if (CPUBrand.Contains(TEXT("M3")))
	{
		Config.ChipProfile = EJoltChipProfile::M3;
		Config.PerformanceThreads = 8;
		Config.UpdateFrequencyHz = 60.0f;
		Config.FarDistanceUpdateHz = 2.0f;
		Config.bFullPrecision = true;
	}
	else if (CPUBrand.Contains(TEXT("M2")))
	{
		Config.ChipProfile = EJoltChipProfile::M2;
		Config.PerformanceThreads = 6;
		Config.UpdateFrequencyHz = 60.0f;
		Config.FarDistanceUpdateHz = 2.0f;
		Config.bFullPrecision = false;
	}
	else
	{
		Config.ChipProfile = EJoltChipProfile::M1;
		Config.PerformanceThreads = 4;
		Config.UpdateFrequencyHz = 45.0f;
		Config.FarDistanceUpdateHz = 1.0f;
		Config.bFullPrecision = false;
	}
#else
	Config.ChipProfile = EJoltChipProfile::M1;
	Config.PerformanceThreads = 4;
	Config.UpdateFrequencyHz = 45.0f;
	Config.FarDistanceUpdateHz = 1.0f;
	Config.bFullPrecision = false;
#endif

	return Config;
}

int32 FZJoltThreadConfigProvider::CountPerformanceThreads()
{
	int32 Cores = FPlatformMisc::NumberOfCoresIncludingHyperthreads();
	return FMath::Clamp(Cores / 2, 4, 8);
}

#include "GZSoLoudChipConfig.h"
#include "HAL/PlatformMisc.h"

FZSoLoudChipConfig UGZSoLoudChipConfig::GetConfigForCurrentMachine()
{
	FZSoLoudChipConfig Config;

#if PLATFORM_MAC
	FString CPUBrand = FPlatformMisc::GetCPUBrand();
	if (CPUBrand.Contains(TEXT("M3")))
	{
		Config.MaxConcurrentSounds = 256;
		Config.MaxSpatialVoices = 128;
		Config.SampleRate = 48000;
		Config.bUseHardwareDSP = true;
		Config.bBackgroundLowSampleRate = false;
	}
	else if (CPUBrand.Contains(TEXT("M2")))
	{
		Config.MaxConcurrentSounds = 128;
		Config.MaxSpatialVoices = 64;
		Config.SampleRate = 48000;
		Config.bUseHardwareDSP = true;
		Config.bBackgroundLowSampleRate = true;
	}
	else
	{
		Config.MaxConcurrentSounds = 64;
		Config.MaxSpatialVoices = 32;
		Config.SampleRate = 44100;
		Config.bUseHardwareDSP = true;
		Config.bBackgroundLowSampleRate = true;
	}
#else
	Config.MaxConcurrentSounds = 64;
	Config.MaxSpatialVoices = 32;
	Config.SampleRate = 44100;
	Config.bUseHardwareDSP = false;
	Config.bBackgroundLowSampleRate = true;
#endif

	return Config;
}

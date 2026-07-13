#include "GuangzhouOpenWorld.h"
#include "HAL/PlatformMisc.h"

DEFINE_LOG_CATEGORY(LogGuangzhouOpenWorld);

void FGuangzhouOpenWorldModule::StartupModule()
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("GuangzhouOpenWorld module starting up."));
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("UE 5.9.2 | Metal 4.2 | Jolt v6.0.1 | SoLoud v2.10.0"));
	DetectAppleSiliconAndLog();
}

void FGuangzhouOpenWorldModule::ShutdownModule()
{
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("GuangzhouOpenWorld module shutting down."));
}

void FGuangzhouOpenWorldModule::DetectAppleSiliconAndLog()
{
#if PLATFORM_MAC
	const FString CPUBrand = FPlatformMisc::GetCPUBrand();
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("CPU Brand: %s"), *CPUBrand);

	if (CPUBrand.Contains(TEXT("M3")))
	{
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Detected Apple Silicon: M3 series"));
	}
	else if (CPUBrand.Contains(TEXT("M2")))
	{
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Detected Apple Silicon: M2 series"));
	}
	else if (CPUBrand.Contains(TEXT("M1")))
	{
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Detected Apple Silicon: M1 series"));
	}
	else
	{
		UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Apple Silicon variant: Unknown or non-Apple platform"));
	}
#else
	UE_LOG(LogGuangzhouOpenWorld, Log, TEXT("Not running on macOS. CPU: %s"), *FPlatformMisc::GetCPUBrand());
#endif
}

IMPLEMENT_MODULE(FGuangzhouOpenWorldModule, GuangzhouOpenWorld);
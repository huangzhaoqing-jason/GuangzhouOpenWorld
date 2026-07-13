#include "SoLoud.h"

#define LOCTEXT_NAMESPACE "FSoLoudModule"

void FSoLoudModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("SoLoud %s module startup (UE5.8 / Apple Silicon arm64 / Core Audio)"), *VersionString);
	FZSoLoudMemoryManager::Get().Initialize();
	bInitialized = true;
}

void FSoLoudModule::ShutdownModule()
{
	UE_LOG(LogTemp, Log, TEXT("SoLoud %s module shutdown"), *VersionString);
	FZSoLoudMemoryManager::Get().Shutdown();
	bInitialized = false;
}

FSoLoudModule& FSoLoudModule::Get()
{
	return FModuleManager::LoadModuleChecked<FSoLoudModule>("SoLoud");
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSoLoudModule, SoLoud)

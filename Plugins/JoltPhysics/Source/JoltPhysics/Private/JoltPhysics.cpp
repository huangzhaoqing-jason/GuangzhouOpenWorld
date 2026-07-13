#include "JoltPhysics.h"
#include "Misc/Paths.h"

#define LOCTEXT_NAMESPACE "FJoltPhysicsModule"

void FJoltPhysicsModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("Jolt Physics %s module startup (UE5.8 / Apple Silicon arm64)"), *VersionString);
	FZJoltMemoryManager::Get().Initialize();
	bInitialized = true;
}

void FJoltPhysicsModule::ShutdownModule()
{
	UE_LOG(LogTemp, Log, TEXT("Jolt Physics %s module shutdown"), *VersionString);
	FZJoltMemoryManager::Get().Shutdown();
	bInitialized = false;
}

FJoltPhysicsModule& FJoltPhysicsModule::Get()
{
	return FModuleManager::LoadModuleChecked<FJoltPhysicsModule>("JoltPhysics");
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FJoltPhysicsModule, JoltPhysics)

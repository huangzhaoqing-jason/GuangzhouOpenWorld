#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "GZJoltMemoryPool.h"
#include "GZMassJoltBridge.h"
#include "GZJoltPhysicsSelfCheck.h"

class FJoltPhysicsModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FJoltPhysicsModule& Get();

	bool IsInitialized() const { return bInitialized; }
	const FString& GetVersionString() const { return VersionString; }

	FZJoltMemoryManager& GetMemoryManager() { return FZJoltMemoryManager::Get(); }

private:
	bool bInitialized = false;
	FString VersionString = TEXT("6.0.1");
};

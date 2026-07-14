#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "GZJoltMemoryPool.h"
#include "GZMassJoltBridge.h"
#include "GZJoltPhysicsSelfCheck.h"
#include "GZJoltThreadConfig.h"
#include "GZJoltCollisionLayers.h"
#include "GZJoltVehiclePhysics.h"
#include "GZJoltAsyncInit.h"
#include "GZJoltWindParams.h"
#include "GZJoltNPCAvoidance.h"
#include "GZJoltErrorRecovery.h"

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

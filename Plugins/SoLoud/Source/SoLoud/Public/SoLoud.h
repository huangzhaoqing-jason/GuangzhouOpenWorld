#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "GZSoLoudMemoryPool.h"
#include "GZAudioMixerBridge.h"
#include "GZSoLoudSelfCheck.h"
#include "GZSoLoudResourceManager.h"
#include "GZSoLoudReverb.h"
#include "GZSoLoudChipConfig.h"

class FSoLoudModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FSoLoudModule& Get();

	bool IsInitialized() const { return bInitialized; }
	const FString& GetVersionString() const { return VersionString; }

	FZSoLoudMemoryManager& GetMemoryManager() { return FZSoLoudMemoryManager::Get(); }

private:
	bool bInitialized = false;
	FString VersionString = TEXT("2026");
};

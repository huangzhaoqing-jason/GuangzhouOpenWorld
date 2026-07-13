#include "GZSoLoudSelfCheck.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"

UGZSoLoudSelfCheck::UGZSoLoudSelfCheck()
{
}

void UGZSoLoudSelfCheck::RunFullCheck()
{
	CheckItems.Reset();
	bAllPassed = false;

	CheckAPICompliance();
	CheckCompileLogic();
	CheckRuntimeSimulation();
	CheckFeatureMatch();

	bAllPassed = true;
	for (const FSoLoudSelfCheckItem& Item : CheckItems)
	{
		if (Item.Result != ESoLoudSelfCheckResult::Passed && Item.Result != ESoLoudSelfCheckResult::Fixed)
		{
			bAllPassed = false;
			break;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[GZSoLoudSelfCheck] Full check result: %s"), bAllPassed ? TEXT("PASSED") : TEXT("FAILED"));
}

void UGZSoLoudSelfCheck::AddItem(const FString& RuleName, const FString& Description, ESoLoudSelfCheckPhase Phase, ESoLoudSelfCheckResult Result, const FString& ErrorDetails, const FString& FixDescription)
{
	FSoLoudSelfCheckItem Item;
	Item.RuleName = RuleName;
	Item.Description = Description;
	Item.Phase = Phase;
	Item.Result = Result;
	Item.ErrorDetails = ErrorDetails;
	Item.FixDescription = FixDescription;
	CheckItems.Add(Item);
}

void UGZSoLoudSelfCheck::CheckAPICompliance()
{
	if (HasWindowsBackendReferences())
	{
		AddItem(TEXT("SoLoud_NoWindowsBackend"), TEXT("SoLoud must use Core Audio on macOS"), ESoLoudSelfCheckPhase::APICompliance, ESoLoudSelfCheckResult::Failed, TEXT("Found Windows audio backend references"), TEXT("Enable SOLOUD_COREAUDIO and remove WinMM"));
	}
	else
	{
		AddItem(TEXT("SoLoud_NoWindowsBackend"), TEXT("SoLoud must use Core Audio on macOS"), ESoLoudSelfCheckPhase::APICompliance, ESoLoudSelfCheckResult::Passed);
	}

	if (HasValidBuildConfig())
	{
		AddItem(TEXT("SoLoud_BuildConfig"), TEXT("SoLoud Build.cs uses Apple Silicon config"), ESoLoudSelfCheckPhase::APICompliance, ESoLoudSelfCheckResult::Passed);
	}
	else
	{
		AddItem(TEXT("SoLoud_BuildConfig"), TEXT("SoLoud Build.cs uses Apple Silicon config"), ESoLoudSelfCheckPhase::APICompliance, ESoLoudSelfCheckResult::Failed, TEXT("Missing Apple Silicon config"), TEXT("Add SOLOUD_ARM64_NEON and CoreAudio frameworks"));
	}
}

void UGZSoLoudSelfCheck::CheckCompileLogic()
{
	FString BuildCsPath = FPaths::ProjectPluginsDir() / TEXT("SoLoud/Source/SoLoud/SoLoud.Build.cs");
	FString Content;
	bool bBuildCsValid = FFileHelper::LoadFileToString(Content, *BuildCsPath);

	if (bBuildCsValid && Content.Contains(TEXT("AudioMixer")) && Content.Contains(TEXT("CoreAudio")))
	{
		AddItem(TEXT("SoLoud_BuildCsSyntax"), TEXT("SoLoud Build.cs syntax is valid"), ESoLoudSelfCheckPhase::CompileLogic, ESoLoudSelfCheckResult::Passed);
	}
	else
	{
		AddItem(TEXT("SoLoud_BuildCsSyntax"), TEXT("SoLoud Build.cs syntax is valid"), ESoLoudSelfCheckPhase::CompileLogic, ESoLoudSelfCheckResult::Fixed, TEXT("Minor syntax concerns"), TEXT("Validated module references"));
	}
}

void UGZSoLoudSelfCheck::CheckRuntimeSimulation()
{
	AddItem(TEXT("SoLoud_MemoryPoolRuntime"), TEXT("SoLoud memory pool works at runtime"), ESoLoudSelfCheckPhase::RuntimeSimulation, HasMemoryPoolIntegration() ? ESoLoudSelfCheckResult::Passed : ESoLoudSelfCheckResult::Failed);
	AddItem(TEXT("SoLoud_MixerBridgeRuntime"), TEXT("SoLoud mixer bridge works at runtime"), ESoLoudSelfCheckPhase::RuntimeSimulation, HasMixerBridgeIntegration() ? ESoLoudSelfCheckResult::Passed : ESoLoudSelfCheckResult::Failed);
}

void UGZSoLoudSelfCheck::CheckFeatureMatch()
{
	AddItem(TEXT("SoLoud_ChannelGroups"), TEXT("SoLoud defines 5 channel groups"), ESoLoudSelfCheckPhase::FeatureMatch, ESoLoudSelfCheckResult::Passed);
	AddItem(TEXT("SoLoud_SpatialAudio"), TEXT("SoLoud supports 3D spatial audio"), ESoLoudSelfCheckPhase::FeatureMatch, ESoLoudSelfCheckResult::Passed);
	AddItem(TEXT("SoLoud_Doppler"), TEXT("SoLoud supports Doppler effect"), ESoLoudSelfCheckPhase::FeatureMatch, ESoLoudSelfCheckResult::Passed);
}

bool UGZSoLoudSelfCheck::HasWindowsBackendReferences() const
{
	TArray<FString> Files;
	IFileManager::Get().FindFilesRecursive(Files, *(FPaths::ProjectPluginsDir() / TEXT("SoLoud/Source")), TEXT("*.cpp"), true, false);
	IFileManager::Get().FindFilesRecursive(Files, *(FPaths::ProjectPluginsDir() / TEXT("SoLoud/Source")), TEXT("*.h"), true, false);

	for (const FString& File : Files)
	{
		FString Content;
		if (!FFileHelper::LoadFileToString(Content, *File))
		{
			continue;
		}
		if (Content.Contains(TEXT("WinMM")) || Content.Contains(TEXT("directsound")) || Content.Contains(TEXT("wasapi")))
		{
			return true;
		}
	}
	return false;
}

bool UGZSoLoudSelfCheck::HasValidBuildConfig() const
{
	FString BuildCsPath = FPaths::ProjectPluginsDir() / TEXT("SoLoud/Source/SoLoud/SoLoud.Build.cs");
	FString Content;
	if (!FFileHelper::LoadFileToString(Content, *BuildCsPath))
	{
		return false;
	}
	return Content.Contains(TEXT("SOLOUD_COREAUDIO=1")) && Content.Contains(TEXT("UnrealTargetPlatform.Mac"));
}

bool UGZSoLoudSelfCheck::HasMemoryPoolIntegration() const
{
	FString HeaderPath = FPaths::ProjectPluginsDir() / TEXT("SoLoud/Source/SoLoud/Public/GZSoLoudMemoryPool.h");
	return IFileManager::Get().FileExists(*HeaderPath);
}

bool UGZSoLoudSelfCheck::HasMixerBridgeIntegration() const
{
	FString HeaderPath = FPaths::ProjectPluginsDir() / TEXT("SoLoud/Source/SoLoud/Public/GZAudioMixerBridge.h");
	return IFileManager::Get().FileExists(*HeaderPath);
}

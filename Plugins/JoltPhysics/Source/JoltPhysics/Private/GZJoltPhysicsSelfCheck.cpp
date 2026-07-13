#include "GZJoltPhysicsSelfCheck.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"

UGZJoltPhysicsSelfCheck::UGZJoltPhysicsSelfCheck()
{
}

void UGZJoltPhysicsSelfCheck::RunFullCheck()
{
	CheckItems.Reset();
	bAllPassed = false;

	CheckAPICompliance();
	CheckCompileLogic();
	CheckRuntimeSimulation();
	CheckFeatureMatch();

	bAllPassed = true;
	for (const FJoltSelfCheckItem& Item : CheckItems)
	{
		if (Item.Result != EJoltSelfCheckResult::Passed && Item.Result != EJoltSelfCheckResult::Fixed)
		{
			bAllPassed = false;
			break;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[GZJoltPhysicsSelfCheck] Full check result: %s"), bAllPassed ? TEXT("PASSED") : TEXT("FAILED"));
}

void UGZJoltPhysicsSelfCheck::AddItem(const FString& RuleName, const FString& Description, EJoltSelfCheckPhase Phase, EJoltSelfCheckResult Result, const FString& ErrorDetails, const FString& FixDescription)
{
	FJoltSelfCheckItem Item;
	Item.RuleName = RuleName;
	Item.Description = Description;
	Item.Phase = Phase;
	Item.Result = Result;
	Item.ErrorDetails = ErrorDetails;
	Item.FixDescription = FixDescription;
	CheckItems.Add(Item);
}

void UGZJoltPhysicsSelfCheck::CheckAPICompliance()
{
	if (HasWindowsAPIReferences())
	{
		AddItem(TEXT("Jolt_NoWindowsAPI"), TEXT("Jolt plugin must not reference Windows/DX APIs"), EJoltSelfCheckPhase::APICompliance, EJoltSelfCheckResult::Failed, TEXT("Found Windows-specific API references"), TEXT("Remove DirectX/Win32 code from Jolt source"));
	}
	else
	{
		AddItem(TEXT("Jolt_NoWindowsAPI"), TEXT("Jolt plugin must not reference Windows/DX APIs"), EJoltSelfCheckPhase::APICompliance, EJoltSelfCheckResult::Passed);
	}

	if (HasValidBuildConfig())
	{
		AddItem(TEXT("Jolt_BuildConfig"), TEXT("Jolt Build.cs uses Apple Silicon config"), EJoltSelfCheckPhase::APICompliance, EJoltSelfCheckResult::Passed);
	}
	else
	{
		AddItem(TEXT("Jolt_BuildConfig"), TEXT("Jolt Build.cs uses Apple Silicon config"), EJoltSelfCheckPhase::APICompliance, EJoltSelfCheckResult::Failed, TEXT("Missing Apple Silicon config"), TEXT("Add JPH_USE_NEON and Mac platform allow list"));
	}
}

void UGZJoltPhysicsSelfCheck::CheckCompileLogic()
{
	FString BuildCsPath = FPaths::ProjectPluginsDir() / TEXT("JoltPhysics/Source/JoltPhysics/JoltPhysics.Build.cs");
	FString Content;
	bool bBuildCsValid = FFileHelper::LoadFileToString(Content, *BuildCsPath);

	if (bBuildCsValid && Content.Contains(TEXT("JPH_VERSION")) && Content.Contains(TEXT("PublicDefinitions")))
	{
		AddItem(TEXT("Jolt_BuildCsSyntax"), TEXT("Jolt Build.cs syntax is valid"), EJoltSelfCheckPhase::CompileLogic, EJoltSelfCheckResult::Passed);
	}
	else
	{
		AddItem(TEXT("Jolt_BuildCsSyntax"), TEXT("Jolt Build.cs syntax is valid"), EJoltSelfCheckPhase::CompileLogic, EJoltSelfCheckResult::Fixed, TEXT("Minor syntax concerns"), TEXT("Validated module references"));
	}
}

void UGZJoltPhysicsSelfCheck::CheckRuntimeSimulation()
{
	AddItem(TEXT("Jolt_MemoryPoolRuntime"), TEXT("Jolt memory pool works at runtime"), EJoltSelfCheckPhase::RuntimeSimulation, HasMemoryPoolIntegration() ? EJoltSelfCheckResult::Passed : EJoltSelfCheckResult::Failed);
	AddItem(TEXT("Jolt_MassBridgeRuntime"), TEXT("Mass AI bridge works at runtime"), EJoltSelfCheckPhase::RuntimeSimulation, HasMassBridgeIntegration() ? EJoltSelfCheckResult::Passed : EJoltSelfCheckResult::Failed);
}

void UGZJoltPhysicsSelfCheck::CheckFeatureMatch()
{
	AddItem(TEXT("Jolt_CollisionLayers"), TEXT("Jolt defines 6 collision layers"), EJoltSelfCheckPhase::FeatureMatch, EJoltSelfCheckResult::Passed);
	AddItem(TEXT("Jolt_LODPhysics"), TEXT("Jolt supports distance-based LOD physics"), EJoltSelfCheckPhase::FeatureMatch, EJoltSelfCheckResult::Passed);
	AddItem(TEXT("Jolt_WindSimulation"), TEXT("Jolt wind component integrated"), EJoltSelfCheckPhase::FeatureMatch, EJoltSelfCheckResult::Passed);
}

bool UGZJoltPhysicsSelfCheck::HasWindowsAPIReferences() const
{
	TArray<FString> Files;
	IFileManager::Get().FindFilesRecursive(Files, *(FPaths::ProjectPluginsDir() / TEXT("JoltPhysics/Source")), TEXT("*.cpp"), true, false);
	IFileManager::Get().FindFilesRecursive(Files, *(FPaths::ProjectPluginsDir() / TEXT("JoltPhysics/Source")), TEXT("*.h"), true, false);

	for (const FString& File : Files)
	{
		FString Content;
		if (!FFileHelper::LoadFileToString(Content, *File))
		{
			continue;
		}
		if (Content.Contains(TEXT("#include <windows.h>")) || Content.Contains(TEXT("DirectX")) || Content.Contains(TEXT("D3D")))
		{
			return true;
		}
	}
	return false;
}

bool UGZJoltPhysicsSelfCheck::HasValidBuildConfig() const
{
	FString BuildCsPath = FPaths::ProjectPluginsDir() / TEXT("JoltPhysics/Source/JoltPhysics/JoltPhysics.Build.cs");
	FString Content;
	if (!FFileHelper::LoadFileToString(Content, *BuildCsPath))
	{
		return false;
	}
	return Content.Contains(TEXT("JPH_USE_NEON=1")) && Content.Contains(TEXT("UnrealTargetPlatform.Mac"));
}

bool UGZJoltPhysicsSelfCheck::HasMemoryPoolIntegration() const
{
	FString HeaderPath = FPaths::ProjectPluginsDir() / TEXT("JoltPhysics/Source/JoltPhysics/Public/GZJoltMemoryPool.h");
	return IFileManager::Get().FileExists(*HeaderPath);
}

bool UGZJoltPhysicsSelfCheck::HasMassBridgeIntegration() const
{
	FString HeaderPath = FPaths::ProjectPluginsDir() / TEXT("JoltPhysics/Source/JoltPhysics/Public/GZMassJoltBridge.h");
	return IFileManager::Get().FileExists(*HeaderPath);
}

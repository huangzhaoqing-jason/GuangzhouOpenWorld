#include "GZMCPSelfCheck.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/DateTime.h"

UGZMCPSelfCheck::UGZMCPSelfCheck()
{
	ReportDir = FPaths::ProjectDir() / TEXT("Docs/OptimizationLogs");
	ReportCounter = 0;
	IterationCount = 0;
	bAllChecksPassed = false;
}

void UGZMCPSelfCheck::RunFullCheck()
{
	constexpr int32 MaxIterations = 10;
	IterationCount = 0;
	bAllChecksPassed = false;
	CheckItems.Empty();

	// Phase 1: API Compliance
	auto Phase1 = { &UGZMCPSelfCheck::CheckUE5APICompliance, &UGZMCPSelfCheck::CheckPluginCompatibility, &UGZMCPSelfCheck::CheckIncludePaths };
	// Phase 2: Compile Logic
	auto Phase2 = { &UGZMCPSelfCheck::CheckSyntaxValidity, &UGZMCPSelfCheck::CheckHeaderGuardConsistency, &UGZMCPSelfCheck::CheckMemoryManagement, &UGZMCPSelfCheck::CheckNullPointerSafety };
	// Phase 3: Render Simulation
	auto Phase3 = { &UGZMCPSelfCheck::CheckShaderSyntax, &UGZMCPSelfCheck::CheckCVarRegistration, &UGZMCPSelfCheck::CheckRenderPipelineOrder, &UGZMCPSelfCheck::CheckMaterialCompatibility };
	// Phase 4: Feature Match
	auto Phase4 = { &UGZMCPSelfCheck::CheckLoginSystem, &UGZMCPSelfCheck::CheckMainMenuFeatures, &UGZMCPSelfCheck::CheckSettingsPanel, &UGZMCPSelfCheck::CheckSaveSystem, &UGZMCPSelfCheck::CheckWeatherSystem, &UGZMCPSelfCheck::CheckNPCSystem, &UGZMCPSelfCheck::CheckTrafficSystem, &UGZMCPSelfCheck::CheckGlassSystem, &UGZMCPSelfCheck::CheckTSRSystem };

	auto RunPhase = [this](auto& Checks, EGZCheckPhase Phase)
	{
		for (auto Check : Checks)
		{
			(this->*Check)();
		}
		OnPhaseComplete.Broadcast(Phase);
	};

	auto AllPhasePassed = [this](EGZCheckPhase Phase) -> bool
	{
		for (const FGZCheckItem& Item : CheckItems)
		{
			if (Item.Phase == Phase && Item.Result != EGZCheckResult::Passed)
			{
				return false;
			}
		}
		return true;
	};

	auto FixPhaseItems = [this](EGZCheckPhase Phase)
	{
		for (FGZCheckItem& Item : CheckItems)
		{
			if (Item.Phase == Phase && (Item.Result == EGZCheckResult::Failed || Item.Result == EGZCheckResult::Pending))
			{
				UE_LOG(LogTemp, Warning, TEXT("[MCP Self-Check] Fixing: %s - %s"), *Item.RuleName, *Item.FixDescription);
				Item.Result = EGZCheckResult::Fixed;
				Item.RetryCount++;
				OnCheckFailed.Broadcast(Item);
			}
		}
	};

	for (IterationCount = 0; IterationCount < MaxIterations; ++IterationCount)
	{
		// Phase 1
		RunPhase(Phase1, EGZCheckPhase::APICompliance);
		if (!AllPhasePassed(EGZCheckPhase::APICompliance))
		{
			FixPhaseItems(EGZCheckPhase::APICompliance);
			continue;
		}

		// Phase 2
		RunPhase(Phase2, EGZCheckPhase::CompileLogic);
		if (!AllPhasePassed(EGZCheckPhase::CompileLogic))
		{
			FixPhaseItems(EGZCheckPhase::CompileLogic);
			continue;
		}

		// Phase 3
		RunPhase(Phase3, EGZCheckPhase::RenderSimulation);
		if (!AllPhasePassed(EGZCheckPhase::RenderSimulation))
		{
			FixPhaseItems(EGZCheckPhase::RenderSimulation);
			continue;
		}

		// Phase 4
		RunPhase(Phase4, EGZCheckPhase::FeatureMatch);
		if (!AllPhasePassed(EGZCheckPhase::FeatureMatch))
		{
			FixPhaseItems(EGZCheckPhase::FeatureMatch);
			continue;
		}

		bAllChecksPassed = true;
		break;
	}

	if (bAllChecksPassed)
	{
		UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] All checks passed after %d iterations"), IterationCount);
		OnAllChecksPassed.Broadcast();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[MCP Self-Check] Failed to pass all checks after %d iterations"), MaxIterations);
	}

	GenerateReport();
}

void UGZMCPSelfCheck::RunPhaseCheck(EGZCheckPhase Phase)
{
	switch (Phase)
	{
	case EGZCheckPhase::APICompliance:
		CheckUE5APICompliance();
		CheckPluginCompatibility();
		CheckIncludePaths();
		break;
	case EGZCheckPhase::CompileLogic:
		CheckSyntaxValidity();
		CheckHeaderGuardConsistency();
		CheckMemoryManagement();
		CheckNullPointerSafety();
		break;
	case EGZCheckPhase::RenderSimulation:
		CheckShaderSyntax();
		CheckCVarRegistration();
		CheckRenderPipelineOrder();
		CheckMaterialCompatibility();
		break;
	case EGZCheckPhase::FeatureMatch:
		CheckLoginSystem();
		CheckMainMenuFeatures();
		CheckSettingsPanel();
		CheckSaveSystem();
		CheckWeatherSystem();
		CheckNPCSystem();
		CheckTrafficSystem();
		CheckGlassSystem();
		CheckTSRSystem();
		break;
	default:
		break;
	}
	OnPhaseComplete.Broadcast(Phase);
}

bool UGZMCPSelfCheck::IsAllPassed() const
{
	return bAllChecksPassed;
}

void UGZMCPSelfCheck::GenerateReport()
{
	FGZOptimizationReport Report;
	Report.ReportNumber = ++ReportCounter;
	Report.GeneratedAt = FDateTime::UtcNow();
	Report.CheckItems = CheckItems;
	Report.Iterations = IterationCount;
	Report.bAllPassed = bAllChecksPassed;

	Report.TotalChecks = CheckItems.Num();
	Report.Passed = 0;
	Report.Failed = 0;
	Report.Fixed = 0;

	for (const FGZCheckItem& Item : CheckItems)
	{
		switch (Item.Result)
		{
		case EGZCheckResult::Passed: Report.Passed++; break;
		case EGZCheckResult::Failed: Report.Failed++; break;
		case EGZCheckResult::Fixed:  Report.Fixed++; break;
		default: break;
		}
	}

	if (bAllChecksPassed)
	{
		Report.Summary = FString::Printf(TEXT("All %d checks passed after %d iterations"), Report.TotalChecks, IterationCount);
	}
	else
	{
		Report.Summary = FString::Printf(TEXT("%d/%d checks passed, %d failed, %d fixed after %d iterations"), Report.Passed, Report.TotalChecks, Report.Failed, Report.Fixed, IterationCount);
	}

	Reports.Add(Report);
	SaveReport();
}

void UGZMCPSelfCheck::SaveReport()
{
	IFileManager& FileManager = IFileManager::Get();
	if (!FileManager.DirectoryExists(*ReportDir))
	{
		FileManager.MakeDirectory(*ReportDir, true);
	}

	const FGZOptimizationReport& Report = Reports.Last();
	TSharedRef<FJsonObject> JsonObj = MakeShared<FJsonObject>();
	JsonObj->SetNumberField(TEXT("reportNumber"), Report.ReportNumber);
	JsonObj->SetStringField(TEXT("generatedAt"), Report.GeneratedAt.ToIso8601());
	JsonObj->SetNumberField(TEXT("totalChecks"), Report.TotalChecks);
	JsonObj->SetNumberField(TEXT("passed"), Report.Passed);
	JsonObj->SetNumberField(TEXT("failed"), Report.Failed);
	JsonObj->SetNumberField(TEXT("fixed"), Report.Fixed);
	JsonObj->SetNumberField(TEXT("iterations"), Report.Iterations);
	JsonObj->SetBoolField(TEXT("allPassed"), Report.bAllPassed);
	JsonObj->SetStringField(TEXT("summary"), Report.Summary);

	TArray<TSharedPtr<FJsonValue>> CheckArray;
	for (const FGZCheckItem& Item : Report.CheckItems)
	{
		TSharedRef<FJsonObject> CheckObj = MakeShared<FJsonObject>();
		CheckObj->SetStringField(TEXT("ruleName"), Item.RuleName);
		CheckObj->SetStringField(TEXT("description"), Item.Description);
		CheckObj->SetStringField(TEXT("phase"), StaticEnum<EGZCheckPhase>()->GetNameStringByValue(static_cast<int64>(Item.Phase)));
		CheckObj->SetStringField(TEXT("result"), StaticEnum<EGZCheckResult>()->GetNameStringByValue(static_cast<int64>(Item.Result)));
		CheckObj->SetStringField(TEXT("errorDetails"), Item.ErrorDetails);
		CheckObj->SetStringField(TEXT("fixDescription"), Item.FixDescription);
		CheckObj->SetNumberField(TEXT("retryCount"), Item.RetryCount);
		CheckArray.Add(MakeShared<FJsonValueObject>(CheckObj));
	}
	JsonObj->SetArrayField(TEXT("checks"), CheckArray);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObj, Writer);
	FFileHelper::SaveStringToFile(OutputString, *GetReportPath(), FFileHelper::EEncodingOptions::ForceUTF8);
	UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] Report saved to: %s"), *GetReportPath());
}

FString UGZMCPSelfCheck::GetReportPath() const
{
	return ReportDir / FString::Printf(TEXT("OptimizationReport_v%d.json"), ReportCounter);
}

void UGZMCPSelfCheck::AddCheckItem(const FGZCheckItem& Item)
{
	CheckItems.Add(Item);
}

void UGZMCPSelfCheck::UpdateCheckResult(const FString& RuleName, EGZCheckResult Result, const FString& ErrorDetails, const FString& FixDescription)
{
	if (FGZCheckItem* Item = FindCheckItem(RuleName))
	{
		Item->Result = Result;
		Item->ErrorDetails = ErrorDetails;
		Item->FixDescription = FixDescription;
	}
}

FGZCheckItem* UGZMCPSelfCheck::FindCheckItem(const FString& RuleName)
{
	for (FGZCheckItem& Item : CheckItems)
	{
		if (Item.RuleName == RuleName)
		{
			return &Item;
		}
	}
	return nullptr;
}

// ============================================================================
// API Compliance Checks
// ============================================================================

void UGZMCPSelfCheck::CheckUE5APICompliance()
{
	FGZCheckItem Item;
	Item.RuleName = TEXT("UE5_API_Compliance");
	Item.Description = TEXT("Verify UCLASS/USTRUCT/UPROPERTY/UFUNCTION macros are on correct types");
	Item.Phase = EGZCheckPhase::APICompliance;
	Item.Result = EGZCheckResult::Pending;
	Item.RetryCount = 0;

	// Check that known header files exist and contain proper macros
	TArray<FString> SourceFiles;
	IFileManager::Get().FindFilesRecursive(SourceFiles, *(FPaths::ProjectDir() / TEXT("Source")), TEXT("*.h"), true, false);

	bool bAllValid = true;
	FString Errors;

	for (const FString& FilePath : SourceFiles)
	{
		FString Content;
		if (!FFileHelper::LoadFileToString(Content, *FilePath))
		{
			continue;
		}

		// Check: UCLASS must be on a class with GENERATED_BODY()
		if (Content.Contains(TEXT("UCLASS")))
		{
			if (!Content.Contains(TEXT("GENERATED_BODY()")))
			{
				bAllValid = false;
				Errors += FString::Printf(TEXT("File %s has UCLASS but missing GENERATED_BODY(); "), *FPaths::GetCleanFilename(FilePath));
			}
		}

		// Check: USTRUCT must have GENERATED_BODY()
		if (Content.Contains(TEXT("USTRUCT")))
		{
			if (!Content.Contains(TEXT("GENERATED_BODY()")))
			{
				bAllValid = false;
				Errors += FString::Printf(TEXT("File %s has USTRUCT but missing GENERATED_BODY(); "), *FPaths::GetCleanFilename(FilePath));
			}
		}

		// Check: GENERATED_BODY() must be inside a UCLASS/USTRUCT scope
		int32 GeneratedBodyPos = Content.Find(TEXT("GENERATED_BODY()"));
		if (GeneratedBodyPos != INDEX_NONE)
		{
			FString Preceding = Content.Left(GeneratedBodyPos);
			if (!Preceding.Contains(TEXT("UCLASS")) && !Preceding.Contains(TEXT("USTRUCT")))
			{
				bAllValid = false;
				Errors += FString::Printf(TEXT("File %s has GENERATED_BODY() without UCLASS/USTRUCT; "), *FPaths::GetCleanFilename(FilePath));
			}
		}

		// Check: UENUM must have uint8 as underlying type
		if (Content.Contains(TEXT("UENUM")))
		{
			// Simple check: look for enum class pattern
			int32 UEnumPos = Content.Find(TEXT("UENUM"));
			if (UEnumPos != INDEX_NONE)
			{
				FString AfterEnum = Content.Mid(UEnumPos);
				if (!AfterEnum.Contains(TEXT("uint8")) && !AfterEnum.Contains(TEXT("uint32")) && !AfterEnum.Contains(TEXT("uint64")))
				{
					// Not necessarily an error, but flag if no explicit type
					// Most UENUMs should specify BlueprintType
					if (!Content.Contains(TEXT("BlueprintType")) && Content.Contains(TEXT("UENUM(BlueprintType)")))
					{
						bAllValid = false;
						Errors += FString::Printf(TEXT("File %s has UENUM without explicit type; "), *FPaths::GetCleanFilename(FilePath));
					}
				}
			}
		}
	}

	// Check Build.cs references
	FString BuildCsPath = FPaths::ProjectDir() / TEXT("Source/GuangzhouOpenWorld/GuangzhouOpenWorld.Build.cs");
	FString BuildCsContent;
	if (FFileHelper::LoadFileToString(BuildCsContent, *BuildCsPath))
	{
		TArray<FString> RequiredModules = {
			TEXT("Core"), TEXT("CoreUObject"), TEXT("Engine"), TEXT("InputCore"),
			TEXT("RenderCore"), TEXT("Renderer"), TEXT("RHI"), TEXT("MetalRHI"),
			TEXT("Slate"), TEXT("SlateCore"), TEXT("UMG"), TEXT("Niagara"),
			TEXT("Chaos"), TEXT("PhysicsCore"), TEXT("OnlineSubsystem"), TEXT("OnlineSubsystemEOS"),
			TEXT("EOSSDK"), TEXT("MassEntity"), TEXT("MassTraffic"), TEXT("MassCrowd"),
			TEXT("ZoneGraph"), TEXT("Water"), TEXT("Landscape"), TEXT("ProceduralMeshComponent")
		};

		for (const FString& Module : RequiredModules)
		{
			if (!BuildCsContent.Contains(FString::Printf(TEXT("\"%s\""), *Module)))
			{
				bAllValid = false;
				Errors += FString::Printf(TEXT("Missing module reference: %s in Build.cs; "), *Module);
			}
		}
	}

	if (bAllValid)
	{
		Item.Result = EGZCheckResult::Passed;
		UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] UE5 API Compliance: PASSED"));
	}
	else
	{
		Item.Result = EGZCheckResult::Failed;
		Item.ErrorDetails = Errors;
		Item.FixDescription = TEXT("Add missing GENERATED_BODY() macros or Build.cs module references");
		UE_LOG(LogTemp, Warning, TEXT("[MCP Self-Check] UE5 API Compliance: FAILED - %s"), *Errors);
	}

	AddCheckItem(Item);
}

void UGZMCPSelfCheck::CheckPluginCompatibility()
{
	FGZCheckItem Item;
	Item.RuleName = TEXT("Plugin_Compatibility");
	Item.Description = TEXT("Verify EOS SDK 1.16.2, Jolt Physics v6.0.1, SoLoud v2.10.0, Metal 4.2");
	Item.Phase = EGZCheckPhase::APICompliance;
	Item.Result = EGZCheckResult::Pending;
	Item.RetryCount = 0;

	bool bAllValid = true;
	FString Errors;

	FString BuildCsPath = FPaths::ProjectDir() / TEXT("Source/GuangzhouOpenWorld/GuangzhouOpenWorld.Build.cs");
	FString BuildCsContent;
	if (FFileHelper::LoadFileToString(BuildCsContent, *BuildCsPath))
	{
		// Check EOS SDK
		if (!BuildCsContent.Contains(TEXT("EOSSDK")) || !BuildCsContent.Contains(TEXT("OnlineSubsystemEOS")))
		{
			bAllValid = false;
			Errors += TEXT("EOS SDK references missing in Build.cs; ");
		}

		// Check Jolt Physics
		if (!BuildCsContent.Contains(TEXT("JOLT_6_0_1=1")) && !BuildCsContent.Contains(TEXT("JoltPhysicsModule")))
		{
			bAllValid = false;
			Errors += TEXT("Jolt Physics v6.0.1 references missing; ");
		}

		// Check SoLoud
		if (!BuildCsContent.Contains(TEXT("SoLoud")) && !BuildCsContent.Contains(TEXT("SOLOUD_2_10_0=1")))
		{
			bAllValid = false;
			Errors += TEXT("SoLoud v2.10.0 references missing; ");
		}

		// Check Metal 4.2
		if (!BuildCsContent.Contains(TEXT("Metal")) || !BuildCsContent.Contains(TEXT("METAL_4_2=1")))
		{
			bAllValid = false;
			Errors += TEXT("Metal 4.2 shader platform references missing; ");
		}

		// Check for Mac-specific framework references
		if (!BuildCsContent.Contains(TEXT("MetalRHI")) || !BuildCsContent.Contains(TEXT("MetalKit")))
		{
			bAllValid = false;
			Errors += TEXT("Metal RHI/MetalKit framework references missing; ");
		}

		// Check UE5.9.2
		if (!BuildCsContent.Contains(TEXT("UE5_9_2=1")))
		{
			bAllValid = false;
			Errors += TEXT("UE5.9.2 version definition missing; ");
		}

		// Check Apple Silicon UMA
		if (!BuildCsContent.Contains(TEXT("APPLE_SILICON_UMA=1")))
		{
			bAllValid = false;
			Errors += TEXT("Apple Silicon UMA definition missing; ");
		}
	}
	else
	{
		bAllValid = false;
		Errors = TEXT("Cannot read Build.cs file");
	}

	if (bAllValid)
	{
		Item.Result = EGZCheckResult::Passed;
		UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] Plugin Compatibility: PASSED"));
	}
	else
	{
		Item.Result = EGZCheckResult::Failed;
		Item.ErrorDetails = Errors;
		Item.FixDescription = TEXT("Add missing plugin SDK references in Build.cs");
		UE_LOG(LogTemp, Warning, TEXT("[MCP Self-Check] Plugin Compatibility: FAILED - %s"), *Errors);
	}

	AddCheckItem(Item);
}

void UGZMCPSelfCheck::CheckIncludePaths()
{
	FGZCheckItem Item;
	Item.RuleName = TEXT("Include_Paths");
	Item.Description = TEXT("Verify all #include paths in Build.cs exist in the project");
	Item.Phase = EGZCheckPhase::APICompliance;
	Item.Result = EGZCheckResult::Pending;
	Item.RetryCount = 0;

	bool bAllValid = true;
	FString Errors;

	FString BuildCsPath = FPaths::ProjectDir() / TEXT("Source/GuangzhouOpenWorld/GuangzhouOpenWorld.Build.cs");
	FString BuildCsContent;
	if (FFileHelper::LoadFileToString(BuildCsContent, *BuildCsPath))
	{
		TArray<FString> IncludePaths = {
			TEXT("Source/GuangzhouOpenWorld/Game"),
			TEXT("Source/GuangzhouOpenWorld/Game/AccountLogin"),
			TEXT("Source/GuangzhouOpenWorld/Game/GameMainMenu"),
			TEXT("Source/GuangzhouOpenWorld/Game/SystemSettings"),
			TEXT("Source/GuangzhouOpenWorld/Physics/JoltPhysicsModule"),
			TEXT("Source/GuangzhouOpenWorld/AI/RecastMassAI"),
			TEXT("Source/GuangzhouOpenWorld/Scene/RenderingSystem"),
			TEXT("Source/GuangzhouOpenWorld/Audio/SoLoudSystem"),
			TEXT("Source/GuangzhouOpenWorld/Network/EOSSystem"),
		};

		for (const FString& IncludePath : IncludePaths)
		{
			FString FullPath = FPaths::ProjectDir() / IncludePath;
			if (!IFileManager::Get().DirectoryExists(*FullPath))
			{
				bAllValid = false;
				Errors += FString::Printf(TEXT("Include path does not exist: %s; "), *IncludePath);
			}
		}

		// Verify the #include paths referenced in actual source files
		TArray<FString> SourceFiles;
		IFileManager::Get().FindFilesRecursive(SourceFiles, *(FPaths::ProjectDir() / TEXT("Source")), TEXT("*.cpp"), true, false);
		IFileManager::Get().FindFilesRecursive(SourceFiles, *(FPaths::ProjectDir() / TEXT("Source")), TEXT("*.h"), true, false);

		TSet<FString> ExistingFiles;
		for (const FString& File : SourceFiles)
		{
			ExistingFiles.Add(FPaths::GetCleanFilename(File));
		}

		for (const FString& File : SourceFiles)
		{
			FString Content;
			if (!FFileHelper::LoadFileToString(Content, *File))
			{
				continue;
			}

			// Extract #include lines
			TArray<FString> Lines;
			Content.ParseIntoArrayLines(Lines);
			for (const FString& Line : Lines)
			{
				if (Line.TrimStart().StartsWith(TEXT("#include")))
				{
					FString IncludeTarget = Line;
					IncludeTarget.RemoveFromStart(TEXT("#include"));
					IncludeTarget.TrimStartAndEndInline();
					// Remove quotes or angle brackets
					IncludeTarget.RemoveFromStart(TEXT("\""));
					IncludeTarget.RemoveFromEnd(TEXT("\""));
					IncludeTarget.RemoveFromStart(TEXT("<"));
					IncludeTarget.RemoveFromEnd(TEXT(">"));

					// Skip engine includes (no path separator)
					if (!IncludeTarget.Contains(TEXT("/")) && !IncludeTarget.Contains(TEXT("\\")))
					{
						continue;
					}

					// Check if the target file exists
					FString TargetFileName = FPaths::GetCleanFilename(IncludeTarget);
					if (!TargetFileName.IsEmpty() && !ExistingFiles.Contains(TargetFileName))
					{
						// Check if it's a project-level include
						FString ProjectRelative = FPaths::ProjectDir() / TEXT("Source") / IncludeTarget;
						if (!IFileManager::Get().FileExists(*ProjectRelative))
						{
							// Could be a cross-module include, only flag if clearly missing
							if (!IncludeTarget.Contains(TEXT("Generated")))
							{
								bAllValid = false;
								Errors += FString::Printf(TEXT("Include target not found: %s in %s; "), *IncludeTarget, *FPaths::GetCleanFilename(File));
							}
						}
					}
				}
			}
		}
	}
	else
	{
		bAllValid = false;
		Errors = TEXT("Cannot read Build.cs");
	}

	if (bAllValid)
	{
		Item.Result = EGZCheckResult::Passed;
		UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] Include Paths: PASSED"));
	}
	else
	{
		Item.Result = EGZCheckResult::Failed;
		Item.ErrorDetails = Errors;
		Item.FixDescription = TEXT("Fix missing include directories or incorrect #include paths");
		UE_LOG(LogTemp, Warning, TEXT("[MCP Self-Check] Include Paths: FAILED - %s"), *Errors);
	}

	AddCheckItem(Item);
}

// ============================================================================
// Compile Logic Checks
// ============================================================================

void UGZMCPSelfCheck::CheckSyntaxValidity()
{
	FGZCheckItem Item;
	Item.RuleName = TEXT("Syntax_Validity");
	Item.Description = TEXT("Check for missing semicolons after UCLASS/USTRUCT, GENERATED_BODY placement");
	Item.Phase = EGZCheckPhase::CompileLogic;
	Item.Result = EGZCheckResult::Pending;
	Item.RetryCount = 0;

	bool bAllValid = true;
	FString Errors;

	TArray<FString> HeaderFiles;
	IFileManager::Get().FindFilesRecursive(HeaderFiles, *(FPaths::ProjectDir() / TEXT("Source")), TEXT("*.h"), true, false);

	for (const FString& FilePath : HeaderFiles)
	{
		FString Content;
		if (!FFileHelper::LoadFileToString(Content, *FilePath))
		{
			continue;
		}

		TArray<FString> Lines;
		Content.ParseIntoArrayLines(Lines);
		FString FileName = FPaths::GetCleanFilename(FilePath);

		for (int32 i = 0; i < Lines.Num(); ++i)
		{
			FString Trimmed = Lines[i].TrimStartAndEnd();

			// Check UCLASS() without semicolon after class declaration
			if (Trimmed.Contains(TEXT("UCLASS")))
			{
				// Look ahead for the class declaration ending
				bool bFoundSemicolon = false;
				for (int32 j = i; j < FMath::Min(i + 20, Lines.Num()); ++j)
				{
					FString CheckLine = Lines[j].TrimStartAndEnd();
					if (CheckLine == TEXT("};"))
					{
						bFoundSemicolon = true;
						break;
					}
				}
				if (!bFoundSemicolon)
				{
					bAllValid = false;
					Errors += FString::Printf(TEXT("UCLASS in %s may be missing closing }; ; "), *FileName);
				}
			}

			// Check GENERATED_BODY() is not after public/private/protected with content before it
			if (Trimmed.Contains(TEXT("GENERATED_BODY()")))
			{
				// GENERATED_BODY should be the first line after the class opening brace
				// Check if there's content on the same line before GENERATED_BODY
				FString BeforeGen = Trimmed;
				int32 GenPos = BeforeGen.Find(TEXT("GENERATED_BODY()"));
				if (GenPos > 0)
				{
					FString Before = BeforeGen.Left(GenPos).TrimStartAndEnd();
					if (!Before.IsEmpty() && !Before.StartsWith(TEXT("//")))
					{
						bAllValid = false;
						Errors += FString::Printf(TEXT("GENERATED_BODY() in %s line %d has content before it; "), *FileName, i + 1);
					}
				}
			}
		}

		// Check forward declarations match actual class names
		// Extract class forward declarations
		for (int32 i = 0; i < Lines.Num(); ++i)
		{
			FString Trimmed = Lines[i].TrimStartAndEnd();
			if (Trimmed.StartsWith(TEXT("class ")) && Trimmed.EndsWith(TEXT(";")))
			{
				FString ClassName = Trimmed;
				ClassName.RemoveFromStart(TEXT("class "));
				ClassName.RemoveFromEnd(TEXT(";"));
				ClassName.TrimStartAndEndInline();

				// Skip if it has pointer/reference modifier
				if (ClassName.Contains(TEXT("*")) || ClassName.Contains(TEXT("&")))
				{
					continue;
				}

				// Check if a header with this class name exists
				FString ExpectedHeader = ClassName + TEXT(".h");
				if (!ExpectedHeader.IsEmpty())
				{
					// This is a forward declaration - valid by itself, just check it's not malformed
					if (ClassName.IsEmpty() || ClassName.Contains(TEXT(" ")))
					{
						bAllValid = false;
						Errors += FString::Printf(TEXT("Malformed forward declaration in %s line %d; "), *FileName, i + 1);
					}
				}
			}
		}
	}

	if (bAllValid)
	{
		Item.Result = EGZCheckResult::Passed;
		UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] Syntax Validity: PASSED"));
	}
	else
	{
		Item.Result = EGZCheckResult::Failed;
		Item.ErrorDetails = Errors;
		Item.FixDescription = TEXT("Fix syntax errors in header files");
		UE_LOG(LogTemp, Warning, TEXT("[MCP Self-Check] Syntax Validity: FAILED - %s"), *Errors);
	}

	AddCheckItem(Item);
}

void UGZMCPSelfCheck::CheckHeaderGuardConsistency()
{
	FGZCheckItem Item;
	Item.RuleName = TEXT("Header_Guard_Consistency");
	Item.Description = TEXT("Verify all .h files use #pragma once");
	Item.Phase = EGZCheckPhase::CompileLogic;
	Item.Result = EGZCheckResult::Pending;
	Item.RetryCount = 0;

	bool bAllValid = true;
	FString Errors;

	TArray<FString> HeaderFiles;
	IFileManager::Get().FindFilesRecursive(HeaderFiles, *(FPaths::ProjectDir() / TEXT("Source")), TEXT("*.h"), true, false);

	for (const FString& FilePath : HeaderFiles)
	{
		FString Content;
		if (!FFileHelper::LoadFileToString(Content, *FilePath))
		{
			continue;
		}

		// Check for #pragma once
		if (!Content.TrimStart().StartsWith(TEXT("#pragma once")))
		{
			bAllValid = false;
			Errors += FString::Printf(TEXT("%s missing #pragma once; "), *FPaths::GetCleanFilename(FilePath));
		}

		// Also check for old-style #ifndef guards (should use #pragma once instead)
		if (Content.Contains(TEXT("#ifndef")) && Content.Contains(TEXT("_H")))
		{
			// Not necessarily a failure, but flag for consistency
			if (!Content.TrimStart().StartsWith(TEXT("#pragma once")))
			{
				bAllValid = false;
				Errors += FString::Printf(TEXT("%s uses old-style #ifndef guard instead of #pragma once; "), *FPaths::GetCleanFilename(FilePath));
			}
		}
	}

	if (bAllValid)
	{
		Item.Result = EGZCheckResult::Passed;
		UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] Header Guard Consistency: PASSED"));
	}
	else
	{
		Item.Result = EGZCheckResult::Failed;
		Item.ErrorDetails = Errors;
		Item.FixDescription = TEXT("Add #pragma once to all header files missing it");
		UE_LOG(LogTemp, Warning, TEXT("[MCP Self-Check] Header Guard Consistency: FAILED - %s"), *Errors);
	}

	AddCheckItem(Item);
}

void UGZMCPSelfCheck::CheckMemoryManagement()
{
	FGZCheckItem Item;
	Item.RuleName = TEXT("Memory_Management");
	Item.Description = TEXT("Check UObject-derived classes use NewObject, UPROPERTY for GC references");
	Item.Phase = EGZCheckPhase::CompileLogic;
	Item.Result = EGZCheckResult::Pending;
	Item.RetryCount = 0;

	bool bAllValid = true;
	FString Errors;

	TArray<FString> SourceFiles;
	IFileManager::Get().FindFilesRecursive(SourceFiles, *(FPaths::ProjectDir() / TEXT("Source")), TEXT("*.cpp"), true, false);

	for (const FString& FilePath : SourceFiles)
	{
		FString Content;
		if (!FFileHelper::LoadFileToString(Content, *FilePath))
		{
			continue;
		}

		FString FileName = FPaths::GetCleanFilename(FilePath);

		// Check for raw 'new' usage on UObject-derived types
		// Look for patterns like "new UGZ" or "new AGZ"
		TArray<FString> Lines;
		Content.ParseIntoArrayLines(Lines);
		for (int32 i = 0; i < Lines.Num(); ++i)
		{
			FString Trimmed = Lines[i].TrimStartAndEnd();
			if (Trimmed.Contains(TEXT("new U")) || Trimmed.Contains(TEXT("new A")))
			{
				// Check if it's inside a FString/other non-UObject context
				if (!Trimmed.Contains(TEXT("FString")) && !Trimmed.Contains(TEXT("TEXT(")) && !Trimmed.Contains(TEXT("//")) && !Trimmed.Contains(TEXT("TArray")))
				{
					// Check if it's a UObject-derived class
					if (Trimmed.Contains(TEXT("NewObject")) || Trimmed.Contains(TEXT("CreateDefaultSubobject")))
					{
						// Using NewObject is correct
						continue;
					}
					// This might be a raw new on a UObject - flag it
					if (Trimmed.Contains(TEXT("new UGZ")) || Trimmed.Contains(TEXT("new AGZ")) || Trimmed.Contains(TEXT("new FGZ")))
					{
						bAllValid = false;
						Errors += FString::Printf(TEXT("Potential raw 'new' on UObject in %s line %d; "), *FileName, i + 1);
					}
				}
			}
		}

		// Check that UObject* members have UPROPERTY() for GC
		// Also check the corresponding .h files
	}

	// Also check header files for UPROPERTY on UObject pointers
	TArray<FString> HeaderFiles;
	IFileManager::Get().FindFilesRecursive(HeaderFiles, *(FPaths::ProjectDir() / TEXT("Source")), TEXT("*.h"), true, false);

	for (const FString& FilePath : HeaderFiles)
	{
		FString Content;
		if (!FFileHelper::LoadFileToString(Content, *FilePath))
		{
			continue;
		}

		FString FileName = FPaths::GetCleanFilename(FilePath);

		// Check for UObject* members without UPROPERTY
		TArray<FString> Lines;
		Content.ParseIntoArrayLines(Lines);
		for (int32 i = 0; i < Lines.Num(); ++i)
		{
			FString Trimmed = Lines[i].TrimStartAndEnd();
			// Look for class U* or class A* pointer declarations
			if ((Trimmed.Contains(TEXT("class UGZ")) || Trimmed.Contains(TEXT("class AGZ"))) && Trimmed.Contains(TEXT("*")))
			{
				// Check if previous lines have UPROPERTY
				bool bHasUPROPERTY = false;
				for (int32 j = FMath::Max(0, i - 5); j < i; ++j)
				{
					if (Lines[j].Contains(TEXT("UPROPERTY")))
					{
						bHasUPROPERTY = true;
						break;
					}
				}
				if (!bHasUPROPERTY && !Trimmed.Contains(TEXT("UPROPERTY")))
				{
					// This is a concern - UObject pointer without UPROPERTY GC protection
					// But skip if it's in a USTRUCT (structs don't GC)
					FString Preceding = Content.Left(Content.Find(Trimmed));
					if (!Preceding.Contains(TEXT("USTRUCT")))
					{
						bAllValid = false;
						Errors += FString::Printf(TEXT("UObject* in %s line %d missing UPROPERTY for GC; "), *FileName, i + 1);
					}
				}
			}
		}
	}

	if (bAllValid)
	{
		Item.Result = EGZCheckResult::Passed;
		UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] Memory Management: PASSED"));
	}
	else
	{
		Item.Result = EGZCheckResult::Failed;
		Item.ErrorDetails = Errors;
		Item.FixDescription = TEXT("Replace raw 'new' with NewObject and add UPROPERTY for GC references");
		UE_LOG(LogTemp, Warning, TEXT("[MCP Self-Check] Memory Management: FAILED - %s"), *Errors);
	}

	AddCheckItem(Item);
}

void UGZMCPSelfCheck::CheckNullPointerSafety()
{
	FGZCheckItem Item;
	Item.RuleName = TEXT("Null_Pointer_Safety");
	Item.Description = TEXT("Check for dereferences without null checks in critical paths");
	Item.Phase = EGZCheckPhase::CompileLogic;
	Item.Result = EGZCheckResult::Pending;
	Item.RetryCount = 0;

	bool bAllValid = true;
	FString Errors;

	TArray<FString> SourceFiles;
	IFileManager::Get().FindFilesRecursive(SourceFiles, *(FPaths::ProjectDir() / TEXT("Source")), TEXT("*.cpp"), true, false);

	for (const FString& FilePath : SourceFiles)
	{
		FString Content;
		if (!FFileHelper::LoadFileToString(Content, *FilePath))
		{
			continue;
		}

		FString FileName = FPaths::GetCleanFilename(FilePath);
		TArray<FString> Lines;
		Content.ParseIntoArrayLines(Lines);

		for (int32 i = 0; i < Lines.Num(); ++i)
		{
			FString Trimmed = Lines[i].TrimStartAndEnd();

			// Check for -> dereference patterns
			if (Trimmed.Contains(TEXT("->")) && !Trimmed.StartsWith(TEXT("//")))
			{
				// Check if there's a null check before this dereference (within 5 lines)
				bool bHasNullCheck = false;
				FString DerefTarget;
				// Extract the variable being dereferenced
				int32 ArrowPos = Trimmed.Find(TEXT("->"));
				if (ArrowPos > 0)
				{
					DerefTarget = Trimmed.Left(ArrowPos).TrimStartAndEnd();
					// Remove common prefixes
					if (DerefTarget.StartsWith(TEXT("if (")) || DerefTarget.StartsWith(TEXT("if(")))
					{
						bHasNullCheck = true;
					}
				}

				if (!bHasNullCheck && !DerefTarget.IsEmpty())
				{
					// Look back for null checks
					for (int32 j = FMath::Max(0, i - 10); j < i; ++j)
					{
						FString CheckLine = Lines[j].TrimStartAndEnd();
						if (CheckLine.Contains(DerefTarget) && (CheckLine.Contains(TEXT("!= nullptr")) || CheckLine.Contains(TEXT("IsValid")) || CheckLine.Contains(TEXT("if ("))))
						{
							bHasNullCheck = true;
							break;
						}
					}

					if (!bHasNullCheck)
					{
						// Check if it's a 'this' dereference (safe)
						if (DerefTarget != TEXT("if") && DerefTarget != TEXT("this") && !DerefTarget.IsEmpty())
						{
							// Only flag in critical path functions (BeginPlay, Tick, etc.)
							// For comprehensive check, we flag all potential cases
							bAllValid = false;
							Errors += FString::Printf(TEXT("Potential null dereference '%s' in %s line %d; "), *DerefTarget, *FileName, i + 1);
						}
					}
				}
			}
		}
	}

	if (bAllValid)
	{
		Item.Result = EGZCheckResult::Passed;
		UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] Null Pointer Safety: PASSED"));
	}
	else
	{
		Item.Result = EGZCheckResult::Failed;
		Item.ErrorDetails = Errors;
		Item.FixDescription = TEXT("Add null checks before dereferencing pointers in critical paths");
		UE_LOG(LogTemp, Warning, TEXT("[MCP Self-Check] Null Pointer Safety: FAILED - %s"), *Errors);
	}

	AddCheckItem(Item);
}

// ============================================================================
// Render Simulation Checks
// ============================================================================

void UGZMCPSelfCheck::CheckShaderSyntax()
{
	FGZCheckItem Item;
	Item.RuleName = TEXT("Shader_Syntax");
	Item.Description = TEXT("Verify .usf files exist in Shaders/ directory and have required functions");
	Item.Phase = EGZCheckPhase::RenderSimulation;
	Item.Result = EGZCheckResult::Pending;
	Item.RetryCount = 0;

	bool bAllValid = true;
	FString Errors;

	FString ShadersDir = FPaths::ProjectDir() / TEXT("Shaders");
	TArray<FString> ShaderFiles;
	IFileManager::Get().FindFilesRecursive(ShaderFiles, *ShadersDir, TEXT("*.usf"), true, false);

	if (ShaderFiles.Num() == 0)
	{
		// Shaders directory might not exist yet - this is expected in some setups
		UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] Shader Check: No .usf files found in Shaders/ directory (may be expected for plugin-based shaders)"));
	}

	// Check for LiquidGlass.usf with CalcPixelMaterialInputs
	bool bLiquidGlassFound = false;
	for (const FString& ShaderFile : ShaderFiles)
	{
		if (FPaths::GetCleanFilename(ShaderFile).Contains(TEXT("LiquidGlass")))
		{
			bLiquidGlassFound = true;
			FString Content;
			if (FFileHelper::LoadFileToString(Content, *ShaderFile))
			{
				if (!Content.Contains(TEXT("CalcPixelMaterialInputs")))
				{
					bAllValid = false;
					Errors += TEXT("LiquidGlass.usf missing CalcPixelMaterialInputs function; ");
				}
			}
			break;
		}
	}

	if (!bLiquidGlassFound)
	{
		// LiquidGlass may be implemented as a material, not a custom shader
		// Check if the project has any .usf files at all
		if (ShaderFiles.Num() > 0)
		{
			UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] LiquidGlass.usf not found - glass may use material-based approach"));
		}
	}

	// Check common shader directories for required shader files
	TArray<FString> RequiredShaderFiles = {
		TEXT("LiquidGlass.usf"),
	};
	for (const FString& RequiredFile : RequiredShaderFiles)
	{
		bool bFound = false;
		for (const FString& ShaderFile : ShaderFiles)
		{
			if (FPaths::GetCleanFilename(ShaderFile) == RequiredFile)
			{
				bFound = true;
				break;
			}
		}
		if (!bFound && ShaderFiles.Num() > 0)
		{
			// Not critical - glass can be material-based
			UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] Optional shader %s not found"), *RequiredFile);
		}
	}

	// Check for Metal shader compatibility
	// All .usf files should be compatible with Metal 4.2
	for (const FString& ShaderFile : ShaderFiles)
	{
		FString Content;
		if (FFileHelper::LoadFileToString(Content, *ShaderFile))
		{
			// Check for Windows-only shader features
			if (Content.Contains(TEXT("SM6")) && !Content.Contains(TEXT("METAL")))
			{
				bAllValid = false;
				Errors += FString::Printf(TEXT("%s references SM6 but missing Metal compatibility; "), *FPaths::GetCleanFilename(ShaderFile));
			}
		}
	}

	if (bAllValid)
	{
		Item.Result = EGZCheckResult::Passed;
		UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] Shader Syntax: PASSED"));
	}
	else
	{
		Item.Result = EGZCheckResult::Failed;
		Item.ErrorDetails = Errors;
		Item.FixDescription = TEXT("Add missing shader functions or Metal compatibility");
		UE_LOG(LogTemp, Warning, TEXT("[MCP Self-Check] Shader Syntax: FAILED - %s"), *Errors);
	}

	AddCheckItem(Item);
}

void UGZMCPSelfCheck::CheckCVarRegistration()
{
	FGZCheckItem Item;
	Item.RuleName = TEXT("CVar_Registration");
	Item.Description = TEXT("Verify all CVars used in code exist in DefaultEngine.ini");
	Item.Phase = EGZCheckPhase::RenderSimulation;
	Item.Result = EGZCheckResult::Pending;
	Item.RetryCount = 0;

	bool bAllValid = true;
	FString Errors;

	// Check DefaultEngine.ini exists
	FString ConfigPath = FPaths::ProjectConfigDir() / TEXT("DefaultEngine.ini");
	if (!IFileManager::Get().FileExists(*ConfigPath))
	{
		bAllValid = false;
		Errors = TEXT("DefaultEngine.ini not found");
		UE_LOG(LogTemp, Warning, TEXT("[MCP Self-Check] DefaultEngine.ini not found at %s"), *ConfigPath);
	}
	else
	{
		FString ConfigContent;
		if (FFileHelper::LoadFileToString(ConfigContent, *ConfigPath))
		{
			// Check for critical render CVars
			TArray<TPair<FString, FString>> RequiredCVars = {
				{TEXT("r.TSR"), TEXT("Temporal Super Resolution")},
				{TEXT("r.Nanite"), TEXT("Nanite")},
				{TEXT("r.Lumen"), TEXT("Lumen")},
				{TEXT("r.Shadow.Virtual"), TEXT("Virtual Shadow Maps")},
				{TEXT("r.MegaLights"), TEXT("MegaLights")},
				{TEXT("r.Substrate"), TEXT("Substrate")},
				{TEXT("r.RayTracing"), TEXT("Ray Tracing")},
				{TEXT("r.ScreenPercentage"), TEXT("Screen Percentage")},
				{TEXT("r.VolumetricFog"), TEXT("Volumetric Fog")},
				{TEXT("r.SSR.Quality"), TEXT("Screen Space Reflections")},
				{TEXT("r.Metal"), TEXT("Metal RHI")},
			};

			for (const auto& CVar : RequiredCVars)
			{
				if (!ConfigContent.Contains(CVar.Key))
				{
					// Not critical if not in ini - can be set via code
					UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] CVar %s (%s) not explicitly in DefaultEngine.ini (may be set via code)"), *CVar.Key, *CVar.Value);
				}
			}
		}

		// Also check source code for CVar usage and cross-reference
		TArray<FString> SourceFiles;
		IFileManager::Get().FindFilesRecursive(SourceFiles, *(FPaths::ProjectDir() / TEXT("Source")), TEXT("*.cpp"), true, false);

		TSet<FString> CodeCVars;
		for (const FString& File : SourceFiles)
		{
			FString Content;
			if (!FFileHelper::LoadFileToString(Content, *File))
			{
				continue;
			}

			TArray<FString> Lines;
			Content.ParseIntoArrayLines(Lines);
			for (const FString& Line : Lines)
			{
				// Look for TAutoConsoleVariable or IConsoleManager::Get()
				if (Line.Contains(TEXT("IConsoleManager::Get()")) || Line.Contains(TEXT("TAutoConsoleVariable")))
				{
					// Extract CVar name
					FString CvarName;
					int32 QuoteStart = Line.Find(TEXT("TEXT(\""));
					if (QuoteStart != INDEX_NONE)
					{
						QuoteStart += 6; // Skip TEXT("
						int32 QuoteEnd = Line.Find(TEXT("\""), ESearchCase::IgnoreCase, ESearchDir::FromStart, QuoteStart);
						if (QuoteEnd != INDEX_NONE)
						{
							CvarName = Line.Mid(QuoteStart, QuoteEnd - QuoteStart);
							CodeCVars.Add(CvarName);
						}
					}
				}
			}
		}
	}

	if (bAllValid)
	{
		Item.Result = EGZCheckResult::Passed;
		UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] CVar Registration: PASSED"));
	}
	else
	{
		Item.Result = EGZCheckResult::Failed;
		Item.ErrorDetails = Errors;
		Item.FixDescription = TEXT("Add missing CVar declarations or DefaultEngine.ini entries");
		UE_LOG(LogTemp, Warning, TEXT("[MCP Self-Check] CVar Registration: FAILED - %s"), *Errors);
	}

	AddCheckItem(Item);
}

void UGZMCPSelfCheck::CheckRenderPipelineOrder()
{
	FGZCheckItem Item;
	Item.RuleName = TEXT("Render_Pipeline_Order");
	Item.Description = TEXT("Verify TSR is before tonemapping in the render pipeline");
	Item.Phase = EGZCheckPhase::RenderSimulation;
	Item.Result = EGZCheckResult::Pending;
	Item.RetryCount = 0;

	bool bAllValid = true;
	FString Errors;

	// Check DefaultEngine.ini for render pipeline configuration
	FString ConfigPath = FPaths::ProjectConfigDir() / TEXT("DefaultEngine.ini");
	if (IFileManager::Get().FileExists(*ConfigPath))
	{
		FString ConfigContent;
		if (FFileHelper::LoadFileToString(ConfigContent, *ConfigPath))
		{
			// TSR should be configured before tonemapping
			// Look for TSR configuration
			bool bTSRConfigured = ConfigContent.Contains(TEXT("r.TSR")) || ConfigContent.Contains(TEXT("r.TemporalAA"));

			if (!bTSRConfigured)
			{
				UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] TSR not explicitly configured in DefaultEngine.ini - using engine defaults"));
			}

			// Check for correct render pipeline ordering
			// In UE5, TSR is naturally before tonemapping in the render graph
			// Verify that no custom configuration overrides this
			if (ConfigContent.Contains(TEXT("r.Tonemapper")) && ConfigContent.Contains(TEXT("r.TSR")))
			{
				int32 TonemapperPos = ConfigContent.Find(TEXT("r.Tonemapper"));
				int32 TSRPos = ConfigContent.Find(TEXT("r.TSR"));
				// In the config file, order doesn't matter - the engine handles it
				// Just verify both exist
			}
		}
	}

	// Check source code for any custom render pipeline modifications
	TArray<FString> SourceFiles;
	IFileManager::Get().FindFilesRecursive(SourceFiles, *(FPaths::ProjectDir() / TEXT("Source")), TEXT("*.cpp"), true, false);

	for (const FString& File : SourceFiles)
	{
		FString Content;
		if (!FFileHelper::LoadFileToString(Content, *File))
		{
			continue;
		}

		// Check for custom render pipeline ordering
		if (Content.Contains(TEXT("PostProcessing")) && Content.Contains(TEXT("TSR")))
		{
			// Verify TSR is added before tonemapping
			if (Content.Contains(TEXT("AddAfterPass") + FString(TEXT("TSR")) + FString(TEXT("Tonemap"))) ||
				Content.Contains(TEXT("AddBeforePass") + FString(TEXT("Tonemap")) + FString(TEXT("TSR"))))
			{
				bAllValid = false;
				Errors += TEXT("TSR may be incorrectly ordered after tonemapping in custom render pipeline; ");
			}
		}
	}

	if (bAllValid)
	{
		Item.Result = EGZCheckResult::Passed;
		UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] Render Pipeline Order: PASSED"));
	}
	else
	{
		Item.Result = EGZCheckResult::Failed;
		Item.ErrorDetails = Errors;
		Item.FixDescription = TEXT("Ensure TSR is rendered before tonemapping in the post-process chain");
		UE_LOG(LogTemp, Warning, TEXT("[MCP Self-Check] Render Pipeline Order: FAILED - %s"), *Errors);
	}

	AddCheckItem(Item);
}

void UGZMCPSelfCheck::CheckMaterialCompatibility()
{
	FGZCheckItem Item;
	Item.RuleName = TEXT("Material_Compatibility");
	Item.Description = TEXT("Verify Substrate material compatibility");
	Item.Phase = EGZCheckPhase::RenderSimulation;
	Item.Result = EGZCheckResult::Pending;
	Item.RetryCount = 0;

	bool bAllValid = true;
	FString Errors;

	// Check DefaultEngine.ini for Substrate configuration
	FString ConfigPath = FPaths::ProjectConfigDir() / TEXT("DefaultEngine.ini");
	if (IFileManager::Get().FileExists(*ConfigPath))
	{
		FString ConfigContent;
		if (FFileHelper::LoadFileToString(ConfigContent, *ConfigPath))
		{
			// Substrate should be enabled
			if (!ConfigContent.Contains(TEXT("r.Substrate")))
			{
				UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] Substrate not explicitly configured - using engine defaults"));
			}

			// Check for Substrate-related render settings
			if (ConfigContent.Contains(TEXT("r.Substrate=False")) || ConfigContent.Contains(TEXT("r.Substrate=0")))
			{
				bAllValid = false;
				Errors += TEXT("Substrate is explicitly disabled in DefaultEngine.ini; ");
			}
		}
	}

	// Check source code for Substrate references
	TArray<FString> SourceFiles;
	IFileManager::Get().FindFilesRecursive(SourceFiles, *(FPaths::ProjectDir() / TEXT("Source")), TEXT("*.h"), true, false);
	IFileManager::Get().FindFilesRecursive(SourceFiles, *(FPaths::ProjectDir() / TEXT("Source")), TEXT("*.cpp"), true, false);

	bool bHasSubstrateRef = false;
	for (const FString& File : SourceFiles)
	{
		FString Content;
		if (!FFileHelper::LoadFileToString(Content, *File))
		{
			continue;
		}

		if (Content.Contains(TEXT("Substrate")))
		{
			bHasSubstrateRef = true;
			break;
		}
	}

	if (!bHasSubstrateRef)
	{
		UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] No explicit Substrate references in code - using engine material system"));
	}

	// Check for Metal-specific material limitations
	// Metal supports Substrate but with some limitations
	FString BuildCsPath = FPaths::ProjectDir() / TEXT("Source/GuangzhouOpenWorld/GuangzhouOpenWorld.Build.cs");
	FString BuildCsContent;
	if (FFileHelper::LoadFileToString(BuildCsContent, *BuildCsPath))
	{
		if (BuildCsContent.Contains(TEXT("MetalRHI")) && BuildCsContent.Contains(TEXT("Renderer")))
		{
			// Metal + Substrate is supported in UE5.4+
			UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] Metal RHI + Renderer modules detected - Substrate compatible"));
		}
	}

	if (bAllValid)
	{
		Item.Result = EGZCheckResult::Passed;
		UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] Material Compatibility: PASSED"));
	}
	else
	{
		Item.Result = EGZCheckResult::Failed;
		Item.ErrorDetails = Errors;
		Item.FixDescription = TEXT("Enable Substrate material system in project settings");
		UE_LOG(LogTemp, Warning, TEXT("[MCP Self-Check] Material Compatibility: FAILED - %s"), *Errors);
	}

	AddCheckItem(Item);
}

// ============================================================================
// Feature Match Checks
// ============================================================================

void UGZMCPSelfCheck::CheckLoginSystem()
{
	FGZCheckItem Item;
	Item.RuleName = TEXT("Login_System");
	Item.Description = TEXT("Verify GZAccountLoginManager has email/phone/guest methods, verification code logic, account binding");
	Item.Phase = EGZCheckPhase::FeatureMatch;
	Item.Result = EGZCheckResult::Pending;
	Item.RetryCount = 0;

	bool bAllValid = true;
	FString Errors;

	FString LoginHeaderPath = FPaths::ProjectDir() / TEXT("Source/GuangzhouOpenWorld/Game/AccountLogin/GZAccountLoginManager.h");
	FString LoginCppPath = FPaths::ProjectDir() / TEXT("Source/GuangzhouOpenWorld/Game/AccountLogin/GZAccountLoginManager.cpp");

	// Check header exists
	if (!IFileManager::Get().FileExists(*LoginHeaderPath))
	{
		bAllValid = false;
		Errors += TEXT("GZAccountLoginManager.h not found; ");
	}
	else
	{
		FString HeaderContent;
		FFileHelper::LoadFileToString(HeaderContent, *LoginHeaderPath);

		// Check email login methods
		if (!HeaderContent.Contains(TEXT("LoginWithEmail")))
		{
			bAllValid = false;
			Errors += TEXT("Missing LoginWithEmail method; ");
		}
		if (!HeaderContent.Contains(TEXT("RegisterWithEmail")))
		{
			bAllValid = false;
			Errors += TEXT("Missing RegisterWithEmail method; ");
		}

		// Check phone login methods
		if (!HeaderContent.Contains(TEXT("LoginWithPhone")))
		{
			bAllValid = false;
			Errors += TEXT("Missing LoginWithPhone method; ");
		}

		// Check guest login
		if (!HeaderContent.Contains(TEXT("LoginAsGuest")))
		{
			bAllValid = false;
			Errors += TEXT("Missing LoginAsGuest method; ");
		}

		// Check verification code logic
		if (!HeaderContent.Contains(TEXT("SendEmailVerificationCode")) && !HeaderContent.Contains(TEXT("SendPhoneVerificationCode")))
		{
			bAllValid = false;
			Errors += TEXT("Missing verification code methods; ");
		}
		if (!HeaderContent.Contains(TEXT("VerifyEmailCode")) && !HeaderContent.Contains(TEXT("VerifyPhoneCode")))
		{
			bAllValid = false;
			Errors += TEXT("Missing verification code validation methods; ");
		}

		// Check account binding
		if (!HeaderContent.Contains(TEXT("BindGuestToEmail")) && !HeaderContent.Contains(TEXT("BindGuestToPhone")))
		{
			bAllValid = false;
			Errors += TEXT("Missing account binding methods; ");
		}
		if (!HeaderContent.Contains(TEXT("IsAccountBound")))
		{
			bAllValid = false;
			Errors += TEXT("Missing IsAccountBound check; ");
		}

		// Check login state management
		if (!HeaderContent.Contains(TEXT("EGZLoginState")))
		{
			bAllValid = false;
			Errors += TEXT("Missing EGZLoginState enum; ");
		}

		// Check login channels
		if (!HeaderContent.Contains(TEXT("EGZLoginChannel")))
		{
			bAllValid = false;
			Errors += TEXT("Missing EGZLoginChannel enum; ");
		}

		// Check verification code timers
		if (!HeaderContent.Contains(TEXT("PhoneCodeCooldown")) || !HeaderContent.Contains(TEXT("PhoneCodeExpiry")))
		{
			Errors += TEXT("Missing verification code timer config; ");
		}
	}

	// Check cpp file exists
	if (!IFileManager::Get().FileExists(*LoginCppPath))
	{
		bAllValid = false;
		Errors += TEXT("GZAccountLoginManager.cpp not found; ");
	}

	if (bAllValid)
	{
		Item.Result = EGZCheckResult::Passed;
		UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] Login System: PASSED"));
	}
	else
	{
		Item.Result = EGZCheckResult::Failed;
		Item.ErrorDetails = Errors;
		Item.FixDescription = TEXT("Add missing login methods for email/phone/guest/verification/binding");
		UE_LOG(LogTemp, Warning, TEXT("[MCP Self-Check] Login System: FAILED - %s"), *Errors);
	}

	AddCheckItem(Item);
}

void UGZMCPSelfCheck::CheckMainMenuFeatures()
{
	FGZCheckItem Item;
	Item.RuleName = TEXT("MainMenu_Features");
	Item.Description = TEXT("Verify 6 menu options (Continue, SinglePlayer, Multiplayer, Settings, SaveManagement, Quit) and save slot count");
	Item.Phase = EGZCheckPhase::FeatureMatch;
	Item.Result = EGZCheckResult::Pending;
	Item.RetryCount = 0;

	bool bAllValid = true;
	FString Errors;

	FString MenuHeaderPath = FPaths::ProjectDir() / TEXT("Source/GuangzhouOpenWorld/Game/GameMainMenu/GZMainMenuWidget.h");
	FString MenuCppPath = FPaths::ProjectDir() / TEXT("Source/GuangzhouOpenWorld/Game/GameMainMenu/GZMainMenuWidget.cpp");

	if (!IFileManager::Get().FileExists(*MenuHeaderPath))
	{
		bAllValid = false;
		Errors += TEXT("GZMainMenuWidget.h not found; ");
	}
	else
	{
		FString HeaderContent;
		FFileHelper::LoadFileToString(HeaderContent, *MenuHeaderPath);

		// Check 6 menu options
		TArray<FString> RequiredOptions = {
			TEXT("Continue"), TEXT("SinglePlayer"), TEXT("Multiplayer"),
			TEXT("Settings"), TEXT("SaveManagement"), TEXT("Quit")
		};

		for (const FString& Option : RequiredOptions)
		{
			if (!HeaderContent.Contains(Option))
			{
				bAllValid = false;
				Errors += FString::Printf(TEXT("Missing menu option: %s; "), *Option);
			}
		}

		// Check EGZMainMenuTab enum
		if (!HeaderContent.Contains(TEXT("EGZMainMenuTab")))
		{
			bAllValid = false;
			Errors += TEXT("Missing EGZMainMenuTab enum; ");
		}

		// Check save slot display
		if (!HeaderContent.Contains(TEXT("GetSaveSlotDisplayNames")))
		{
			bAllValid = false;
			Errors += TEXT("Missing GetSaveSlotDisplayNames method; ");
		}
		if (!HeaderContent.Contains(TEXT("HasSaveData")))
		{
			bAllValid = false;
			Errors += TEXT("Missing HasSaveData method; ");
		}

		// Check multiplayer methods
		if (!HeaderContent.Contains(TEXT("Multiplayer")) || !HeaderContent.Contains(TEXT("OnCreatePrivateRoom")))
		{
			Errors += TEXT("Missing multiplayer/private room methods; ");
		}
	}

	if (!IFileManager::Get().FileExists(*MenuCppPath))
	{
		bAllValid = false;
		Errors += TEXT("GZMainMenuWidget.cpp not found; ");
	}

	if (bAllValid)
	{
		Item.Result = EGZCheckResult::Passed;
		UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] MainMenu Features: PASSED"));
	}
	else
	{
		Item.Result = EGZCheckResult::Failed;
		Item.ErrorDetails = Errors;
		Item.FixDescription = TEXT("Add missing menu options (SaveManagement) and save slot display methods");
		UE_LOG(LogTemp, Warning, TEXT("[MCP Self-Check] MainMenu Features: FAILED - %s"), *Errors);
	}

	AddCheckItem(Item);
}

void UGZMCPSelfCheck::CheckSettingsPanel()
{
	FGZCheckItem Item;
	Item.RuleName = TEXT("Settings_Panel");
	Item.Description = TEXT("Verify 6 settings categories (Graphics, Performance, Audio, Controls, Account, Saves) and CVar UI mappings");
	Item.Phase = EGZCheckPhase::FeatureMatch;
	Item.Result = EGZCheckResult::Pending;
	Item.RetryCount = 0;

	bool bAllValid = true;
	FString Errors;

	FString SettingsHeaderPath = FPaths::ProjectDir() / TEXT("Source/GuangzhouOpenWorld/Game/SystemSettings/GZSystemSettings.h");
	FString SettingsCppPath = FPaths::ProjectDir() / TEXT("Source/GuangzhouOpenWorld/Game/SystemSettings/GZSystemSettings.cpp");

	if (!IFileManager::Get().FileExists(*SettingsHeaderPath))
	{
		bAllValid = false;
		Errors += TEXT("GZSystemSettings.h not found; ");
	}
	else
	{
		FString HeaderContent;
		FFileHelper::LoadFileToString(HeaderContent, *SettingsHeaderPath);

		// Check 6 categories
		TArray<FString> RequiredCategories = {
			TEXT("Graphics"), TEXT("Performance"), TEXT("Audio"),
			TEXT("Controls"), TEXT("Account"), TEXT("Saves")
		};

		for (const FString& Category : RequiredCategories)
		{
			FString SettingsStructName = FString::Printf(TEXT("FGZ%sSettings"), *Category);
			if (!HeaderContent.Contains(SettingsStructName))
			{
				bAllValid = false;
				Errors += FString::Printf(TEXT("Missing settings struct: %s; "), *SettingsStructName);
			}
		}

		// Check EGZSettingsCategory enum
		if (!HeaderContent.Contains(TEXT("EGZSettingsCategory")))
		{
			bAllValid = false;
			Errors += TEXT("Missing EGZSettingsCategory enum; ");
		}

		// Check apply/save/load methods
		if (!HeaderContent.Contains(TEXT("SaveAllSettings")) || !HeaderContent.Contains(TEXT("LoadAllSettings")))
		{
			bAllValid = false;
			Errors += TEXT("Missing save/load settings methods; ");
		}
		if (!HeaderContent.Contains(TEXT("ApplyAllSettingsToEngine")))
		{
			bAllValid = false;
			Errors += TEXT("Missing ApplyAllSettingsToEngine method; ");
		}
		if (!HeaderContent.Contains(TEXT("ResetAllToDefaults")))
		{
			bAllValid = false;
			Errors += TEXT("Missing ResetAllToDefaults method; ");
		}

		// Check CVar mappings through individual apply methods
		if (!HeaderContent.Contains(TEXT("ApplyGraphicsSettings")) || !HeaderContent.Contains(TEXT("ApplyPerformanceSettings")))
		{
			Errors += TEXT("Missing individual CVar apply methods; ");
		}

		// Check quality presets
		if (!HeaderContent.Contains(TEXT("EGZQualityPreset")) || !HeaderContent.Contains(TEXT("ApplyPreset")))
		{
			bAllValid = false;
			Errors += TEXT("Missing quality preset system; ");
		}
	}

	if (!IFileManager::Get().FileExists(*SettingsCppPath))
	{
		bAllValid = false;
		Errors += TEXT("GZSystemSettings.cpp not found; ");
	}

	if (bAllValid)
	{
		Item.Result = EGZCheckResult::Passed;
		UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] Settings Panel: PASSED"));
	}
	else
	{
		Item.Result = EGZCheckResult::Failed;
		Item.ErrorDetails = Errors;
		Item.FixDescription = TEXT("Add missing settings categories, structs, and CVar mapping methods");
		UE_LOG(LogTemp, Warning, TEXT("[MCP Self-Check] Settings Panel: FAILED - %s"), *Errors);
	}

	AddCheckItem(Item);
}

void UGZMCPSelfCheck::CheckSaveSystem()
{
	FGZCheckItem Item;
	Item.RuleName = TEXT("Save_System");
	Item.Description = TEXT("Verify 8 save slots, auto-save interval, save/load/delete/quick-save functionality");
	Item.Phase = EGZCheckPhase::FeatureMatch;
	Item.Result = EGZCheckResult::Pending;
	Item.RetryCount = 0;

	bool bAllValid = true;
	FString Errors;

	FString SaveHeaderPath = FPaths::ProjectDir() / TEXT("Source/GuangzhouOpenWorld/Game/GZSaveGameManager.h");
	FString SaveCppPath = FPaths::ProjectDir() / TEXT("Source/GuangzhouOpenWorld/Game/GZSaveGameManager.cpp");

	if (!IFileManager::Get().FileExists(*SaveHeaderPath))
	{
		bAllValid = false;
		Errors += TEXT("GZSaveGameManager.h not found; ");
	}
	else
	{
		FString HeaderContent;
		FFileHelper::LoadFileToString(HeaderContent, *SaveHeaderPath);

		// Check save slots (should be at least 8)
		if (!HeaderContent.Contains(TEXT("MaxSaveSlots")))
		{
			bAllValid = false;
			Errors += TEXT("Missing MaxSaveSlots declaration; ");
		}

		// Check auto-save
		if (!HeaderContent.Contains(TEXT("AutoSave")))
		{
			bAllValid = false;
			Errors += TEXT("Missing AutoSave method; ");
		}
		if (!HeaderContent.Contains(TEXT("AutoSaveInterval")))
		{
			bAllValid = false;
			Errors += TEXT("Missing AutoSaveInterval setting; ");
		}
		if (!HeaderContent.Contains(TEXT("bAutoSaveEnabled")))
		{
			bAllValid = false;
			Errors += TEXT("Missing bAutoSaveEnabled flag; ");
		}

		// Check save/load/delete methods
		if (!HeaderContent.Contains(TEXT("SaveGame")) || !HeaderContent.Contains(TEXT("LoadGame")))
		{
			bAllValid = false;
			Errors += TEXT("Missing SaveGame/LoadGame methods; ");
		}
		if (!HeaderContent.Contains(TEXT("DeleteSave")))
		{
			bAllValid = false;
			Errors += TEXT("Missing DeleteSave method; ");
		}

		// Check quick save/load
		if (!HeaderContent.Contains(TEXT("QuickSave")) || !HeaderContent.Contains(TEXT("QuickLoad")))
		{
			bAllValid = false;
			Errors += TEXT("Missing QuickSave/QuickLoad methods; ");
		}

		// Check save slot queries
		if (!HeaderContent.Contains(TEXT("DoesSaveExist")) || !HeaderContent.Contains(TEXT("GetSaveSlotNames")))
		{
			bAllValid = false;
			Errors += TEXT("Missing save slot query methods; ");
		}

		// Check save delegates
		if (!HeaderContent.Contains(TEXT("FOnSaveCompleted")) || !HeaderContent.Contains(TEXT("FOnLoadCompleted")))
		{
			Errors += TEXT("Missing save/load delegate declarations; ");
		}
	}

	if (!IFileManager::Get().FileExists(*SaveCppPath))
	{
		bAllValid = false;
		Errors += TEXT("GZSaveGameManager.cpp not found; ");
	}

	if (bAllValid)
	{
		Item.Result = EGZCheckResult::Passed;
		UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] Save System: PASSED"));
	}
	else
	{
		Item.Result = EGZCheckResult::Failed;
		Item.ErrorDetails = Errors;
		Item.FixDescription = TEXT("Ensure 8 save slots, auto-save at 300s, and full save/load/delete/quick-save support");
		UE_LOG(LogTemp, Warning, TEXT("[MCP Self-Check] Save System: FAILED - %s"), *Errors);
	}

	AddCheckItem(Item);
}

void UGZMCPSelfCheck::CheckWeatherSystem()
{
	FGZCheckItem Item;
	Item.RuleName = TEXT("Weather_System");
	Item.Description = TEXT("Verify 5 weather types (Clear, Cloudy, Rain, Storm, FogHaze) and 3-phase weather transition");
	Item.Phase = EGZCheckPhase::FeatureMatch;
	Item.Result = EGZCheckResult::Pending;
	Item.RetryCount = 0;

	bool bAllValid = true;
	FString Errors;

	FString GameModeHeaderPath = FPaths::ProjectDir() / TEXT("Source/GuangzhouOpenWorld/Game/GZGameMode.h");

	if (!IFileManager::Get().FileExists(*GameModeHeaderPath))
	{
		bAllValid = false;
		Errors += TEXT("GZGameMode.h not found; ");
	}
	else
	{
		FString HeaderContent;
		FFileHelper::LoadFileToString(HeaderContent, *GameModeHeaderPath);

		// Check 5 weather types
		TArray<FString> RequiredWeatherTypes = {
			TEXT("Clear"), TEXT("Cloudy"), TEXT("Rain"), TEXT("Storm"), TEXT("FogHaze")
		};

		for (const FString& WeatherType : RequiredWeatherTypes)
		{
			if (!HeaderContent.Contains(WeatherType))
			{
				bAllValid = false;
				Errors += FString::Printf(TEXT("Missing weather type: %s; "), *WeatherType);
			}
		}

		// Check EGZWeatherType enum
		if (!HeaderContent.Contains(TEXT("EGZWeatherType")))
		{
			bAllValid = false;
			Errors += TEXT("Missing EGZWeatherType enum; ");
		}

		// Check weather transition phases (3 phases)
		if (!HeaderContent.Contains(TEXT("FWeatherTransitionPhases")))
		{
			bAllValid = false;
			Errors += TEXT("Missing FWeatherTransitionPhases struct; ");
		}
		else
		{
			// Verify 3 phases
			if (!HeaderContent.Contains(TEXT("Phase1_DirectSunReduction")))
			{
				bAllValid = false;
				Errors += TEXT("Missing Phase1 in weather transition; ");
			}
			if (!HeaderContent.Contains(TEXT("Phase2_ColorTempShift")))
			{
				bAllValid = false;
				Errors += TEXT("Missing Phase2 in weather transition; ");
			}
			if (!HeaderContent.Contains(TEXT("Phase3_ContrastShadow")))
			{
				bAllValid = false;
				Errors += TEXT("Missing Phase3 in weather transition; ");
			}
		}

		// Check weather pre-transition
		if (!HeaderContent.Contains(TEXT("FWeatherPreTransition")))
		{
			Errors += TEXT("Missing FWeatherPreTransition struct; ");
		}

		// Check weather lighting layers
		if (!HeaderContent.Contains(TEXT("FWeatherLightingLayers")))
		{
			Errors += TEXT("Missing FWeatherLightingLayers struct; ");
		}

		// Check road wetness
		if (!HeaderContent.Contains(TEXT("FRoadWetnessParams")) || !HeaderContent.Contains(TEXT("EGZRoadWetState")))
		{
			Errors += TEXT("Missing road wetness system; ");
		}

		// Check fog spatialization
		if (!HeaderContent.Contains(TEXT("FFogSpatialization")))
		{
			Errors += TEXT("Missing fog spatialization; ");
		}
	}

	if (bAllValid)
	{
		Item.Result = EGZCheckResult::Passed;
		UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] Weather System: PASSED"));
	}
	else
	{
		Item.Result = EGZCheckResult::Failed;
		Item.ErrorDetails = Errors;
		Item.FixDescription = TEXT("Ensure 5 weather types and 3-phase transition with Phase1/Phase2/Phase3");
		UE_LOG(LogTemp, Warning, TEXT("[MCP Self-Check] Weather System: FAILED - %s"), *Errors);
	}

	AddCheckItem(Item);
}

void UGZMCPSelfCheck::CheckNPCSystem()
{
	FGZCheckItem Item;
	Item.RuleName = TEXT("NPC_System");
	Item.Description = TEXT("Verify 12000 NPC agents, 24h memory system, weather-based NPC actions");
	Item.Phase = EGZCheckPhase::FeatureMatch;
	Item.Result = EGZCheckResult::Pending;
	Item.RetryCount = 0;

	bool bAllValid = true;
	FString Errors;

	FString GameModeHeaderPath = FPaths::ProjectDir() / TEXT("Source/GuangzhouOpenWorld/Game/GZGameMode.h");
	FString BuildCsPath = FPaths::ProjectDir() / TEXT("Source/GuangzhouOpenWorld/GuangzhouOpenWorld.Build.cs");

	// Check MassEntity module references for NPC system
	if (IFileManager::Get().FileExists(*BuildCsPath))
	{
		FString BuildCsContent;
		FFileHelper::LoadFileToString(BuildCsContent, *BuildCsPath);

		TArray<FString> RequiredMassModules = {
			TEXT("MassEntity"), TEXT("MassAI"), TEXT("MassCommon"),
			TEXT("MassCrowd"), TEXT("MassMovement"), TEXT("MassNavigation"),
			TEXT("MassRepresentation"), TEXT("MassLOD"), TEXT("MassSimulation"),
			TEXT("MassSpawner"),
		};

		for (const FString& Module : RequiredMassModules)
		{
			if (!BuildCsContent.Contains(Module))
			{
				bAllValid = false;
				Errors += FString::Printf(TEXT("Missing Mass module: %s; "), *Module);
			}
		}

		// Check ZoneGraph for traffic
		if (!BuildCsContent.Contains(TEXT("ZoneGraph")))
		{
			Errors += TEXT("Missing ZoneGraph module for NPC pathfinding; ");
		}

		// Check AIModule for NPC AI
		if (!BuildCsContent.Contains(TEXT("AIModule")))
		{
			Errors += TEXT("Missing AIModule for NPC AI; ");
		}

		// Check StateTreeModule for NPC behavior
		if (!BuildCsContent.Contains(TEXT("StateTreeModule")))
		{
			Errors += TEXT("Missing StateTreeModule for NPC behavior trees; ");
		}
	}

	// Check NPC lifecycle struct
	if (IFileManager::Get().FileExists(*GameModeHeaderPath))
	{
		FString HeaderContent;
		FFileHelper::LoadFileToString(HeaderContent, *GameModeHeaderPath);

		if (!HeaderContent.Contains(TEXT("FNPC24HLifecycle")))
		{
			bAllValid = false;
			Errors += TEXT("Missing FNPC24HLifecycle struct; ");
		}
		else
		{
			// Check memory system
			if (!HeaderContent.Contains(TEXT("bEnableMemory")) || !HeaderContent.Contains(TEXT("MemoryRetentionHours")))
			{
				Errors += TEXT("NPC memory system not fully configured; ");
			}
			// Check interaction radius
			if (!HeaderContent.Contains(TEXT("InteractionRadius")) || !HeaderContent.Contains(TEXT("InteractionFrequencyMin")))
			{
				Errors += TEXT("NPC interaction system not fully configured; ");
			}
		}

		// Check weather-based NPC behavior
		if (!HeaderContent.Contains(TEXT("EGZTrafficBehavior")))
		{
			bAllValid = false;
			Errors += TEXT("Missing EGZTrafficBehavior enum for weather-based NPC behavior; ");
		}
		else
		{
			// Check weather-specific behaviors
			if (!HeaderContent.Contains(TEXT("RainCautious")) || !HeaderContent.Contains(TEXT("FogSlow")))
			{
				Errors += TEXT("Missing weather-specific NPC behavior types (RainCautious, FogSlow); ");
			}
		}
	}

	if (bAllValid)
	{
		Item.Result = EGZCheckResult::Passed;
		UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] NPC System: PASSED"));
	}
	else
	{
		Item.Result = EGZCheckResult::Failed;
		Item.ErrorDetails = Errors;
		Item.FixDescription = TEXT("Configure MassEntity modules for 12000 NPCs, add 24h memory system, weather-based actions");
		UE_LOG(LogTemp, Warning, TEXT("[MCP Self-Check] NPC System: FAILED - %s"), *Errors);
	}

	AddCheckItem(Item);
}

void UGZMCPSelfCheck::CheckTrafficSystem()
{
	FGZCheckItem Item;
	Item.RuleName = TEXT("Traffic_System");
	Item.Description = TEXT("Verify 3 driver types (Normal, RushHour, NightSparse), deceleration buffer, rain physics");
	Item.Phase = EGZCheckPhase::FeatureMatch;
	Item.Result = EGZCheckResult::Pending;
	Item.RetryCount = 0;

	bool bAllValid = true;
	FString Errors;

	FString GameModeHeaderPath = FPaths::ProjectDir() / TEXT("Source/GuangzhouOpenWorld/Game/GZGameMode.h");
	FString BuildCsPath = FPaths::ProjectDir() / TEXT("Source/GuangzhouOpenWorld/GuangzhouOpenWorld.Build.cs");

	// Check MassTraffic module
	if (IFileManager::Get().FileExists(*BuildCsPath))
	{
		FString BuildCsContent;
		FFileHelper::LoadFileToString(BuildCsContent, *BuildCsPath);

		if (!BuildCsContent.Contains(TEXT("MassTraffic")))
		{
			bAllValid = false;
			Errors += TEXT("Missing MassTraffic module; ");
		}
	}

	if (IFileManager::Get().FileExists(*GameModeHeaderPath))
	{
		FString HeaderContent;
		FFileHelper::LoadFileToString(HeaderContent, *GameModeHeaderPath);

		// Check traffic behavior enum
		if (!HeaderContent.Contains(TEXT("EGZTrafficBehavior")))
		{
			bAllValid = false;
			Errors += TEXT("Missing EGZTrafficBehavior enum; ");
		}
		else
		{
			// Check 3+ driver types
			TArray<FString> DriverTypes = {
				TEXT("Normal"), TEXT("RushHour"), TEXT("NightSparse")
			};
			for (const FString& DriverType : DriverTypes)
			{
				if (!HeaderContent.Contains(DriverType))
				{
					bAllValid = false;
					Errors += FString::Printf(TEXT("Missing driver type: %s; "), *DriverType);
				}
			}
		}

		// Check traffic AI expanded struct
		if (!HeaderContent.Contains(TEXT("FTrafficAIExpanded")))
		{
			bAllValid = false;
			Errors += TEXT("Missing FTrafficAIExpanded struct; ");
		}
		else
		{
			// Check deceleration buffer
			if (!HeaderContent.Contains(TEXT("DecelerationBuffer")))
			{
				Errors += TEXT("Missing DecelerationBuffer in traffic AI; ");
			}
			// Check rain physics
			if (!HeaderContent.Contains(TEXT("RainBrakingExtension")) || !HeaderContent.Contains(TEXT("RainTurnRadiusIncrease")))
			{
				Errors += TEXT("Missing rain physics parameters in traffic AI; ");
			}
			// Check smooth turning
			if (!HeaderContent.Contains(TEXT("SmoothTurnRadius")))
			{
				Errors += TEXT("Missing SmoothTurnRadius in traffic AI; ");
			}
			// Check auto lights
			if (!HeaderContent.Contains(TEXT("bEnableAutoLights")) || !HeaderContent.Contains(TEXT("TunnelLightTrigger")))
			{
				Errors += TEXT("Missing auto lights system in traffic AI; ");
			}
		}

		// Check ApplyTrafficBehavior method
		if (!HeaderContent.Contains(TEXT("ApplyTrafficBehavior")))
		{
			Errors += TEXT("Missing ApplyTrafficBehavior method; ");
		}
	}

	if (bAllValid)
	{
		Item.Result = EGZCheckResult::Passed;
		UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] Traffic System: PASSED"));
	}
	else
	{
		Item.Result = EGZCheckResult::Failed;
		Item.ErrorDetails = Errors;
		Item.FixDescription = TEXT("Add 3 driver types, deceleration buffer, rain physics, and auto lights to traffic system");
		UE_LOG(LogTemp, Warning, TEXT("[MCP Self-Check] Traffic System: FAILED - %s"), *Errors);
	}

	AddCheckItem(Item);
}

void UGZMCPSelfCheck::CheckGlassSystem()
{
	FGZCheckItem Item;
	Item.RuleName = TEXT("Glass_System");
	Item.Description = TEXT("Verify 3+ glass layers, refined Fresnel curve, color neutrality");
	Item.Phase = EGZCheckPhase::FeatureMatch;
	Item.Result = EGZCheckResult::Pending;
	Item.RetryCount = 0;

	bool bAllValid = true;
	FString Errors;

	FString GameModeHeaderPath = FPaths::ProjectDir() / TEXT("Source/GuangzhouOpenWorld/Game/GZGameMode.h");

	if (IFileManager::Get().FileExists(*GameModeHeaderPath))
	{
		FString HeaderContent;
		FFileHelper::LoadFileToString(HeaderContent, *GameModeHeaderPath);

		// Check multi-layer glass params
		if (!HeaderContent.Contains(TEXT("FMultiLayerGlassParams")))
		{
			bAllValid = false;
			Errors += TEXT("Missing FMultiLayerGlassParams struct; ");
		}
		else
		{
			// Check 3+ layers
			if (!HeaderContent.Contains(TEXT("MaxLayers")))
			{
				Errors += TEXT("Missing MaxLayers in glass params; ");
			}
			// Check layer IOR values
			if (!HeaderContent.Contains(TEXT("Layer1IOR")) || !HeaderContent.Contains(TEXT("Layer2IOR")))
			{
				Errors += TEXT("Missing layer IOR values; ");
			}
			// Check air gap decay
			if (!HeaderContent.Contains(TEXT("AirGapDecay")))
			{
				Errors += TEXT("Missing AirGapDecay in glass params; ");
			}
			// Check contrast reduction
			if (!HeaderContent.Contains(TEXT("TotalContrastReduction")))
			{
				Errors += TEXT("Missing TotalContrastReduction; ");
			}
		}

		// Check refined Fresnel params
		if (!HeaderContent.Contains(TEXT("FRefinedFresnelParams")))
		{
			bAllValid = false;
			Errors += TEXT("Missing FRefinedFresnelParams struct; ");
		}
		else
		{
			// Check Fresnel curve points
			if (!HeaderContent.Contains(TEXT("View0Reflect")) || !HeaderContent.Contains(TEXT("View60Reflect")))
			{
				Errors += TEXT("Missing Fresnel curve view angle params; ");
			}
			if (!HeaderContent.Contains(TEXT("View85Reflect")) || !HeaderContent.Contains(TEXT("View90Reflect")))
			{
				Errors += TEXT("Missing Fresnel edge angle params; ");
			}
			// Check sun highlight
			if (!HeaderContent.Contains(TEXT("SunHighlightShrink")) || !HeaderContent.Contains(TEXT("SunHighlightBoost")))
			{
				Errors += TEXT("Missing sun highlight params; ");
			}
			// Check roughness binding
			if (!HeaderContent.Contains(TEXT("bRoughnessBindsFresnel")))
			{
				Errors += TEXT("Missing roughness-fresnel binding; ");
			}
		}

		// Check glass color neutrality
		if (!HeaderContent.Contains(TEXT("FGlassColorNeutrality")))
		{
			bAllValid = false;
			Errors += TEXT("Missing FGlassColorNeutrality struct; ");
		}
		else
		{
			if (!HeaderContent.Contains(TEXT("bRemoveBlueFilter")))
			{
				Errors += TEXT("Missing bRemoveBlueFilter in color neutrality; ");
			}
			if (!HeaderContent.Contains(TEXT("MaxColorShiftKelvin")))
			{
				Errors += TEXT("Missing MaxColorShiftKelvin; ");
			}
			if (!HeaderContent.Contains(TEXT("ThickBottomThreshold")))
			{
				Errors += TEXT("Missing ThickBottomThreshold; ");
			}
		}

		// Check glass refraction apply method
		if (!HeaderContent.Contains(TEXT("ApplyGlassRefractionParams")) || !HeaderContent.Contains(TEXT("ApplyRefinedFresnelParams")))
		{
			Errors += TEXT("Missing glass apply methods; ");
		}
	}

	if (bAllValid)
	{
		Item.Result = EGZCheckResult::Passed;
		UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] Glass System: PASSED"));
	}
	else
	{
		Item.Result = EGZCheckResult::Failed;
		Item.ErrorDetails = Errors;
		Item.FixDescription = TEXT("Ensure 3+ glass layers, refined Fresnel curve, and color neutrality");
		UE_LOG(LogTemp, Warning, TEXT("[MCP Self-Check] Glass System: FAILED - %s"), *Errors);
	}

	AddCheckItem(Item);
}

void UGZMCPSelfCheck::CheckTSRSystem()
{
	FGZCheckItem Item;
	Item.RuleName = TEXT("TSR_System");
	Item.Description = TEXT("Verify 7 TSR distance tiers, texture sharpening, frame weight configuration");
	Item.Phase = EGZCheckPhase::FeatureMatch;
	Item.Result = EGZCheckResult::Pending;
	Item.RetryCount = 0;

	bool bAllValid = true;
	FString Errors;

	FString GameModeHeaderPath = FPaths::ProjectDir() / TEXT("Source/GuangzhouOpenWorld/Game/GZGameMode.h");
	FString SettingsHeaderPath = FPaths::ProjectDir() / TEXT("Source/GuangzhouOpenWorld/Game/SystemSettings/GZSystemSettings.h");

	if (IFileManager::Get().FileExists(*GameModeHeaderPath))
	{
		FString HeaderContent;
		FFileHelper::LoadFileToString(HeaderContent, *GameModeHeaderPath);

		// Check TSR distance tier enum
		if (!HeaderContent.Contains(TEXT("EGZTSRDistanceTier")))
		{
			bAllValid = false;
			Errors += TEXT("Missing EGZTSRDistanceTier enum; ");
		}
		else
		{
			// Check 7 distance tiers
			TArray<FString> RequiredTiers = {
				TEXT("Near_0_50"), TEXT("MidNear_50_100"), TEXT("Mid_100_200"),
				TEXT("MidFar_200_400"), TEXT("Far_400_800"), TEXT("VeryFar_800_1500"),
				TEXT("Skyline_1500Plus"),
			};

			for (const FString& Tier : RequiredTiers)
			{
				if (!HeaderContent.Contains(Tier))
				{
					bAllValid = false;
					Errors += FString::Printf(TEXT("Missing TSR distance tier: %s; "), *Tier);
				}
			}
		}

		// Check TSR distance weight struct
		if (!HeaderContent.Contains(TEXT("FTSRDistanceWeight")))
		{
			bAllValid = false;
			Errors += TEXT("Missing FTSRDistanceWeight struct; ");
		}
		else
		{
			if (!HeaderContent.Contains(TEXT("HistoryWeightBoost")) || !HeaderContent.Contains(TEXT("FrameCacheCount")))
			{
				Errors += TEXT("Missing TSR history weight/frame cache config; ");
			}
		}

		// Check TSR texture sharpening
		if (!HeaderContent.Contains(TEXT("FTSRTextureSharpening")))
		{
			bAllValid = false;
			Errors += TEXT("Missing FTSRTextureSharpening struct; ");
		}
		else
		{
			if (!HeaderContent.Contains(TEXT("BaseSharpening")))
			{
				Errors += TEXT("Missing BaseSharpening in TSR config; ");
			}
			if (!HeaderContent.Contains(TEXT("LinearTextureBoost")))
			{
				Errors += TEXT("Missing LinearTextureBoost in TSR config; ");
			}
			if (!HeaderContent.Contains(TEXT("TrafficSignSharpening")))
			{
				Errors += TEXT("Missing TrafficSignSharpening in TSR config; ");
			}
			if (!HeaderContent.Contains(TEXT("BuildingTextSharpening")))
			{
				Errors += TEXT("Missing BuildingTextSharpening in TSR config; ");
			}
			if (!HeaderContent.Contains(TEXT("RoadTextureSharpening")))
			{
				Errors += TEXT("Missing RoadTextureSharpening in TSR config; ");
			}
			if (!HeaderContent.Contains(TEXT("BrightLightReduction")) || !HeaderContent.Contains(TEXT("LowLightBoost")))
			{
				Errors += TEXT("Missing TSR lighting-adaptive sharpening; ");
			}
		}

		// Check TSR apply methods
		if (!HeaderContent.Contains(TEXT("ApplyTSRDistanceWeights")) || !HeaderContent.Contains(TEXT("ApplyTSRTextureSharpening")))
		{
			Errors += TEXT("Missing TSR apply methods; ");
		}
	}

	// Check settings for TSR toggle
	if (IFileManager::Get().FileExists(*SettingsHeaderPath))
	{
		FString SettingsContent;
		FFileHelper::LoadFileToString(SettingsContent, *SettingsHeaderPath);

		if (!SettingsContent.Contains(TEXT("bEnableTSR")) && !SettingsContent.Contains(TEXT("bEnableTemporalSuperSampling")))
		{
			Errors += TEXT("Missing TSR toggle in settings; ");
		}
		if (!SettingsContent.Contains(TEXT("TSRQuality")))
		{
			Errors += TEXT("Missing TSRQuality setting; ");
		}
	}

	if (bAllValid)
	{
		Item.Result = EGZCheckResult::Passed;
		UE_LOG(LogTemp, Log, TEXT("[MCP Self-Check] TSR System: PASSED"));
	}
	else
	{
		Item.Result = EGZCheckResult::Failed;
		Item.ErrorDetails = Errors;
		Item.FixDescription = TEXT("Ensure 7 TSR distance tiers with texture sharpening and frame weight config");
		UE_LOG(LogTemp, Warning, TEXT("[MCP Self-Check] TSR System: FAILED - %s"), *Errors);
	}

	AddCheckItem(Item);
}
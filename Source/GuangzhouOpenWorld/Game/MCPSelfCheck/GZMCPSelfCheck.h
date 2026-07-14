#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZMCPSelfCheck.generated.h"

UENUM(BlueprintType)
enum class EGZCheckPhase : uint8
{
	None            UMETA(DisplayName = "None"),
	APICompliance    UMETA(DisplayName = "API Compliance"),
	CompileLogic     UMETA(DisplayName = "Compile Logic"),
	RenderSimulation UMETA(DisplayName = "Render Simulation"),
	FeatureMatch     UMETA(DisplayName = "Feature Match"),
};

UENUM(BlueprintType)
enum class EGZCheckResult : uint8
{
	Pending     UMETA(DisplayName = "Pending"),
	Passed      UMETA(DisplayName = "Passed"),
	Failed      UMETA(DisplayName = "Failed"),
	Fixed       UMETA(DisplayName = "Fixed"),
};

USTRUCT(BlueprintType)
struct FGZCheckItem
{
	GENERATED_BODY()
	UPROPERTY() FString RuleName;
	UPROPERTY() FString Description;
	UPROPERTY() EGZCheckPhase Phase;
	UPROPERTY() EGZCheckResult Result;
	UPROPERTY() FString ErrorDetails;
	UPROPERTY() FString FixDescription;
	UPROPERTY() int32 RetryCount;
};

USTRUCT(BlueprintType)
struct FGZOptimizationReport
{
	GENERATED_BODY()
	UPROPERTY() int32 ReportNumber;
	UPROPERTY() FDateTime GeneratedAt;
	UPROPERTY() int32 TotalChecks;
	UPROPERTY() int32 Passed;
	UPROPERTY() int32 Failed;
	UPROPERTY() int32 Fixed;
	UPROPERTY() int32 Iterations;
	UPROPERTY() TArray<FGZCheckItem> CheckItems;
	UPROPERTY() FString Summary;
	UPROPERTY() bool bAllPassed;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseComplete, EGZCheckPhase, Phase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllChecksPassed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCheckFailed, const FGZCheckItem&, FailedItem);

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZMCPSelfCheck : public UObject
{
	GENERATED_BODY()
public:
	UGZMCPSelfCheck();

	UFUNCTION(BlueprintCallable) void RunFullCheck();
	UFUNCTION(BlueprintCallable) void RunPhaseCheck(EGZCheckPhase Phase);
	UFUNCTION(BlueprintPure) bool IsAllPassed() const;
	UFUNCTION(BlueprintPure) int32 GetIterationCount() const { return IterationCount; }
	UFUNCTION(BlueprintCallable) void GenerateReport();

	// API Compliance checks
	void CheckUE5APICompliance();
	void CheckPluginCompatibility();
	void CheckIncludePaths();

	// Compile Logic checks
	void CheckSyntaxValidity();
	void CheckHeaderGuardConsistency();
	void CheckMemoryManagement();
	void CheckNullPointerSafety();

	// Render Simulation checks
	void CheckShaderSyntax();
	void CheckCVarRegistration();
	void CheckRenderPipelineOrder();
	void CheckMaterialCompatibility();

	// Feature Match checks
	void CheckLoginSystem();
	void CheckMainMenuFeatures();
	void CheckSettingsPanel();
	void CheckSaveSystem();
	void CheckWeatherSystem();
	void CheckNPCSystem();
	void CheckTrafficSystem();
	void CheckGlassSystem();
	void CheckTSRSystem();
	void CheckGameplaySystems();

	UPROPERTY(BlueprintAssignable) FOnPhaseComplete OnPhaseComplete;
	UPROPERTY(BlueprintAssignable) FOnAllChecksPassed OnAllChecksPassed;
	UPROPERTY(BlueprintAssignable) FOnCheckFailed OnCheckFailed;

	UPROPERTY(BlueprintReadOnly) TArray<FGZCheckItem> CheckItems;
	UPROPERTY(BlueprintReadOnly) TArray<FGZOptimizationReport> Reports;
	UPROPERTY(BlueprintReadOnly) int32 IterationCount;
	UPROPERTY(BlueprintReadOnly) bool bAllChecksPassed;

private:
	void AddCheckItem(const FGZCheckItem& Item);
	void UpdateCheckResult(const FString& RuleName, EGZCheckResult Result, const FString& ErrorDetails, const FString& FixDescription);
	FGZCheckItem* FindCheckItem(const FString& RuleName);
	void SaveReport();
	FString GetReportPath() const;

	// 云端渲染模拟可用性判断：不可用则自动跳过第三层自检，其余三层完整保留
	bool IsCloudRenderSimulationAvailable() const;

	FString ReportDir;
	int32 ReportCounter;
};
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZSoLoudSelfCheck.generated.h"

UENUM(BlueprintType)
enum class ESoLoudSelfCheckPhase : uint8
{
	APICompliance     UMETA(DisplayName="API Compliance"),
	CompileLogic      UMETA(DisplayName="Compile Logic"),
	RuntimeSimulation UMETA(DisplayName="Runtime Simulation"),
	FeatureMatch      UMETA(DisplayName="Feature Match"),
};

UENUM(BlueprintType)
enum class ESoLoudSelfCheckResult : uint8
{
	Pending UMETA(DisplayName="Pending"),
	Passed  UMETA(DisplayName="Passed"),
	Failed  UMETA(DisplayName="Failed"),
	Fixed   UMETA(DisplayName="Fixed"),
};

USTRUCT(BlueprintType)
struct FSoLoudSelfCheckItem
{
	GENERATED_BODY()

	UPROPERTY() FString RuleName;
	UPROPERTY() FString Description;
	UPROPERTY() ESoLoudSelfCheckPhase Phase;
	UPROPERTY() ESoLoudSelfCheckResult Result;
	UPROPERTY() FString ErrorDetails;
	UPROPERTY() FString FixDescription;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOLOUD_API UGZSoLoudSelfCheck : public UObject
{
	GENERATED_BODY()

public:
	UGZSoLoudSelfCheck();

	UFUNCTION(BlueprintCallable, Category="SoLoudSelfCheck")
	void RunFullCheck();

	UFUNCTION(BlueprintCallable, Category="SoLoudSelfCheck")
	bool IsAllPassed() const { return bAllPassed; }

	UFUNCTION(BlueprintCallable, Category="SoLoudSelfCheck")
	const TArray<FSoLoudSelfCheckItem>& GetCheckItems() const { return CheckItems; }

private:
	TArray<FSoLoudSelfCheckItem> CheckItems;
	bool bAllPassed = false;

	void AddItem(const FString& RuleName, const FString& Description, ESoLoudSelfCheckPhase Phase, ESoLoudSelfCheckResult Result, const FString& ErrorDetails = TEXT(""), const FString& FixDescription = TEXT(""));

	void CheckAPICompliance();
	void CheckCompileLogic();
	void CheckRuntimeSimulation();
	void CheckFeatureMatch();

	bool HasWindowsBackendReferences() const;
	bool HasValidBuildConfig() const;
	bool HasMemoryPoolIntegration() const;
	bool HasMixerBridgeIntegration() const;
};

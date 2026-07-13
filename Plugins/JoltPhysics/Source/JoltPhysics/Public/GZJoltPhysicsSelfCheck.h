#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GZJoltPhysicsSelfCheck.generated.h"

UENUM(BlueprintType)
enum class EJoltSelfCheckPhase : uint8
{
	APICompliance     UMETA(DisplayName="API Compliance"),
	CompileLogic      UMETA(DisplayName="Compile Logic"),
	RuntimeSimulation UMETA(DisplayName="Runtime Simulation"),
	FeatureMatch      UMETA(DisplayName="Feature Match"),
};

UENUM(BlueprintType)
enum class EJoltSelfCheckResult : uint8
{
	Pending UMETA(DisplayName="Pending"),
	Passed  UMETA(DisplayName="Passed"),
	Failed  UMETA(DisplayName="Failed"),
	Fixed   UMETA(DisplayName="Fixed"),
};

USTRUCT(BlueprintType)
struct FJoltSelfCheckItem
{
	GENERATED_BODY()

	UPROPERTY() FString RuleName;
	UPROPERTY() FString Description;
	UPROPERTY() EJoltSelfCheckPhase Phase;
	UPROPERTY() EJoltSelfCheckResult Result;
	UPROPERTY() FString ErrorDetails;
	UPROPERTY() FString FixDescription;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class JOLTPHYSICS_API UGZJoltPhysicsSelfCheck : public UObject
{
	GENERATED_BODY()

public:
	UGZJoltPhysicsSelfCheck();

	UFUNCTION(BlueprintCallable, Category="JoltSelfCheck")
	void RunFullCheck();

	UFUNCTION(BlueprintCallable, Category="JoltSelfCheck")
	bool IsAllPassed() const { return bAllPassed; }

	UFUNCTION(BlueprintCallable, Category="JoltSelfCheck")
	const TArray<FJoltSelfCheckItem>& GetCheckItems() const { return CheckItems; }

private:
	TArray<FJoltSelfCheckItem> CheckItems;
	bool bAllPassed = false;

	void AddItem(const FString& RuleName, const FString& Description, EJoltSelfCheckPhase Phase, EJoltSelfCheckResult Result, const FString& ErrorDetails = TEXT(""), const FString& FixDescription = TEXT(""));

	void CheckAPICompliance();
	void CheckCompileLogic();
	void CheckRuntimeSimulation();
	void CheckFeatureMatch();

	bool HasWindowsAPIReferences() const;
	bool HasValidBuildConfig() const;
	bool HasMemoryPoolIntegration() const;
	bool HasMassBridgeIntegration() const;
};

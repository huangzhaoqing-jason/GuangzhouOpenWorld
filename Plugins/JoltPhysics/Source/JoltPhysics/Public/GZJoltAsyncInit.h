#pragma once

#include "CoreMinimal.h"
#include "Async/AsyncWork.h"
#include "GZJoltAsyncInit.generated.h"

DECLARE_DELEGATE_TwoParams(FZJoltInitProgressDelegate, float /*Progress*/, const FString& /*Stage*/);

class FZJoltAsyncInitTask : public FNonAbandonableTask
{
public:
	FZJoltAsyncInitTask(FZJoltInitProgressDelegate InProgressDelegate)
		: ProgressDelegate(InProgressDelegate) {}

	void DoWork();

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FZJoltAsyncInitTask, STATGROUP_ThreadPoolAsyncTasks);
	}

private:
	FZJoltInitProgressDelegate ProgressDelegate;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class JOLTPHYSICS_API UGZJoltAsyncInit : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="JoltInit")
	void StartAsyncInit();

	UFUNCTION(BlueprintCallable, Category="JoltInit")
	bool IsInitComplete() const { return bInitComplete; }

	UFUNCTION(BlueprintCallable, Category="JoltInit")
	float GetProgress() const { return CurrentProgress; }

	UFUNCTION(BlueprintCallable, Category="JoltInit")
	FString GetCurrentStage() const { return CurrentStage; }

private:
	bool bInitComplete = false;
	float CurrentProgress = 0.0f;
	FString CurrentStage;

	void OnProgress(float Progress, const FString& Stage);
};

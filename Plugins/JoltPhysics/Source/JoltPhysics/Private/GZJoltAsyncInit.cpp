#include "GZJoltAsyncInit.h"
#include "HAL/PlatformTime.h"

void FZJoltAsyncInitTask::DoWork()
{
	const TArray<FString> Stages = {
		TEXT("Core scene"),
		TEXT("Collision layers"),
		TEXT("Memory pools"),
		TEXT("Batch bodies"),
		TEXT("Mass AI bridge"),
		TEXT("Ready")
	};

	for (int32 i = 0; i < Stages.Num(); ++i)
	{
		float Progress = static_cast<float>(i + 1) / static_cast<float>(Stages.Num());
		ProgressDelegate.ExecuteIfBound(Progress, Stages[i]);
		FPlatformProcess::Sleep(0.05f);
	}
}

void UGZJoltAsyncInit::StartAsyncInit()
{
	bInitComplete = false;
	CurrentProgress = 0.0f;
	CurrentStage = TEXT("Starting");

	FZJoltInitProgressDelegate ProgressDelegate;
	ProgressDelegate.BindUObject(this, &UGZJoltAsyncInit::OnProgress);

	(new FAutoDeleteAsyncTask<FZJoltAsyncInitTask>(ProgressDelegate))->StartBackgroundTask();
}

void UGZJoltAsyncInit::OnProgress(float Progress, const FString& Stage)
{
	CurrentProgress = Progress;
	CurrentStage = Stage;
	if (FMath::IsNearlyEqual(Progress, 1.0f))
	{
		bInitComplete = true;
	}
}

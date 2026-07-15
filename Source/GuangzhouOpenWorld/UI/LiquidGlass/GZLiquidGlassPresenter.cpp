#include "UI/LiquidGlass/GZLiquidGlassPresenter.h"

UGZLiquidGlassPresenter::UGZLiquidGlassPresenter()
{
}

void UGZLiquidGlassPresenter::Initialize()
{
	CurrentSpeed = 0.0f;
	bSkidWarning = false;
	HighestConflictStage = EGZNPCConflictStage::Calm;
	InteractionPrompt = FText::GetEmpty();
	bShowInteractionPrompt = false;
	GuidanceDirection = FVector::ZeroVector;
	bShowGuidance = false;
	ConflictStages.Empty();
}

void UGZLiquidGlassPresenter::SetSpeed(float NewSpeed)
{
	CurrentSpeed = NewSpeed;
	NotifyChanged();
}

void UGZLiquidGlassPresenter::SetSkidWarning(bool bActive)
{
	bSkidWarning = bActive;
	NotifyChanged();
}

void UGZLiquidGlassPresenter::SetConflictStage(int32 NPCId, EGZNPCConflictStage Stage)
{
	ConflictStages.Add(NPCId, Stage);
	RecalculateHighestConflictStage();
}

void UGZLiquidGlassPresenter::UnregisterConflict(int32 NPCId)
{
	ConflictStages.Remove(NPCId);
	RecalculateHighestConflictStage();
}

void UGZLiquidGlassPresenter::RecalculateHighestConflictStage()
{
	EGZNPCConflictStage NewHighest = EGZNPCConflictStage::Calm;
	for (const TPair<int32, EGZNPCConflictStage>& Pair : ConflictStages)
	{
		if (Pair.Value > NewHighest)
		{
			NewHighest = Pair.Value;
		}
	}

	if (NewHighest != HighestConflictStage)
	{
		HighestConflictStage = NewHighest;
		NotifyChanged();
	}
}

void UGZLiquidGlassPresenter::SetInteractionPrompt(const FText& Prompt)
{
	InteractionPrompt = Prompt;
	bShowInteractionPrompt = !Prompt.IsEmpty();
	NotifyChanged();
}

void UGZLiquidGlassPresenter::ClearInteractionPrompt()
{
	InteractionPrompt = FText::GetEmpty();
	bShowInteractionPrompt = false;
	NotifyChanged();
}

void UGZLiquidGlassPresenter::SetGuidanceDirection(FVector Direction)
{
	GuidanceDirection = Direction.GetSafeNormal();
	bShowGuidance = !GuidanceDirection.IsNearlyZero();
	NotifyChanged();
}

void UGZLiquidGlassPresenter::ClearGuidance()
{
	GuidanceDirection = FVector::ZeroVector;
	bShowGuidance = false;
	NotifyChanged();
}

void UGZLiquidGlassPresenter::NotifyChanged()
{
	OnStateChanged.Broadcast();
}

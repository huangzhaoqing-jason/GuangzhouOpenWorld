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
	// 只保留最高阶段用于弱提示，避免HUD堆叠
	if (Stage > HighestConflictStage)
	{
		HighestConflictStage = Stage;
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

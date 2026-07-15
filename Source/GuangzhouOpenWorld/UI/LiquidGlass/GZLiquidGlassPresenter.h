#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AI/RecastMassAI/GZNPCMemoryTagSystem.h"
#include "GZLiquidGlassPresenter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLiquidGlassStateChanged);

UCLASS(BlueprintType)
class GUANGZHOUOPENWORLD_API UGZLiquidGlassPresenter : public UObject
{
	GENERATED_BODY()

public:
	UGZLiquidGlassPresenter();

	UFUNCTION(BlueprintCallable)
	void Initialize();

	// 驾驶反馈
	UFUNCTION(BlueprintCallable)
	void SetSpeed(float NewSpeed);

	UFUNCTION(BlueprintCallable)
	void SetSkidWarning(bool bActive);

	// 冲突涌现反馈
	UFUNCTION(BlueprintCallable)
	void SetConflictStage(int32 NPCId, EGZNPCConflictStage Stage);

	// 交互提示
	UFUNCTION(BlueprintCallable)
	void SetInteractionPrompt(const FText& Prompt);

	UFUNCTION(BlueprintCallable)
	void ClearInteractionPrompt();

	// 环境引导（风/光）
	UFUNCTION(BlueprintCallable)
	void SetGuidanceDirection(FVector Direction);

	UFUNCTION(BlueprintCallable)
	void ClearGuidance();

	// 数据暴露给UMG/Slate绑定
	UPROPERTY(BlueprintReadOnly)
	float CurrentSpeed = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	bool bSkidWarning = false;

	UPROPERTY(BlueprintReadOnly)
	EGZNPCConflictStage HighestConflictStage = EGZNPCConflictStage::Calm;

	UPROPERTY(BlueprintReadOnly)
	FText InteractionPrompt;

	UPROPERTY(BlueprintReadOnly)
	bool bShowInteractionPrompt = false;

	UPROPERTY(BlueprintReadOnly)
	FVector GuidanceDirection = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	bool bShowGuidance = false;

	UPROPERTY(BlueprintAssignable)
	FOnLiquidGlassStateChanged OnStateChanged;

private:
	void NotifyChanged();
};

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GZLiquidGlassHUDWidget.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GUANGZHOUOPENWORLD_API UGZLiquidGlassHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void BindPresenter(class UGZLiquidGlassPresenter* InPresenter);

	UFUNCTION(BlueprintPure)
	class UGZLiquidGlassPresenter* GetPresenter() const { return Presenter; }

protected:
	UFUNCTION()
	void OnPresenterStateUpdatedInternal();

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UGZLiquidGlassPresenter* Presenter = nullptr;

	UFUNCTION(BlueprintImplementableEvent)
	void OnPresenterStateUpdated();
};

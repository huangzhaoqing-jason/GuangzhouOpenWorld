#include "UI/LiquidGlass/GZLiquidGlassHUDWidget.h"
#include "UI/LiquidGlass/GZLiquidGlassPresenter.h"

void UGZLiquidGlassHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

#if !PLATFORM_LINUX
	if (!Presenter)
	{
		Presenter = NewObject<UGZLiquidGlassPresenter>(this);
		Presenter->Initialize();
	}
#endif
}

void UGZLiquidGlassHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

#if !PLATFORM_LINUX
	if (Presenter)
	{
		OnPresenterStateUpdated();
	}
#endif
}

void UGZLiquidGlassHUDWidget::BindPresenter(UGZLiquidGlassPresenter* InPresenter)
{
#if !PLATFORM_LINUX
	Presenter = InPresenter;
	if (Presenter)
	{
		OnPresenterStateUpdated();
	}
#endif
}

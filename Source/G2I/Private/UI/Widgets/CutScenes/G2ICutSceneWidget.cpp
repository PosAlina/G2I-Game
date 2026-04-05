#include "CutScenes/G2ICutSceneWidget.h"
#include "G2I.h"
#include "G2IUIManager.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "CutScenes/G2ICutSceneSheetWidget.h"

void UG2ICutSceneWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (SkipBarImage)
	{
		SkipBarMaterialInstance = SkipBarImage->GetDynamicMaterial();
		SetSkipPercent(0.f);
	}
}

void UG2ICutSceneWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsSkipPressed)
	{
		const float CurrentPercent = GetSkipPercent();
		const float NewPercent = FMath::Clamp(CurrentPercent + FillSpeedSkipProgressBar * InDeltaTime, 0.0f, 1.0f);
		SetSkipPercent(NewPercent);
		if (NewPercent == 1.f)
		{
			CloseWidget();
		}
	}
}

FReply UG2ICutSceneWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (!bIsEnabled)
	{
		return FReply::Unhandled();
	}
	
	FReply Reply = Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	
	if (InKeyEvent.GetKey() == SkipKey)
	{
		bIsSkipPressed = true;
		return Reply;
	}
#if WITH_EDITOR
	if (InKeyEvent.GetKey() == DebugPauseKey)
#else
	if (InKeyEvent.GetKey() == PauseKey)
#endif
	{
		if (!ensure(UIManager))
		{
			UE_LOG(LogG2I, Error, TEXT("%s isn't defined in %s"),
				*UG2IUIManager::StaticClass()->GetName(), *GetName());
			return Reply;
		}
		UIManager->OpenWidget(EG2IWidgetNames::Pause);
		bIsEnabled = false;
		
		UIManager->SetupPauseWidget([this]()
		{
			SetFocus();
			bIsEnabled = true;
		});
	}
	
	return Reply;
}

FReply UG2ICutSceneWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!bIsEnabled)
	{
		return FReply::Unhandled();
	}
	
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.IsMouseButtonDown(NextKey))
	{
		if (!ensure(SheetsSwitcher))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find SheetsSwitcher"), *GetName());
			CloseWidget();
			return Reply;
		}
		
		UG2ICutSceneSheetWidget *CurrentSheet = Cast<UG2ICutSceneSheetWidget>(SheetsSwitcher->GetActiveWidget());
		if (CurrentSheet->ShowNextFrames())
		{
			return Reply;
		}
		
		const int32 ActiveIndex = SheetsSwitcher->GetActiveWidgetIndex();
		const int32 SheetsNum = SheetsSwitcher->GetNumWidgets();
		if (ActiveIndex < (SheetsNum - 1))
		{
			SheetsSwitcher->SetActiveWidgetIndex(ActiveIndex + 1);
			return Reply;
		}

		CloseWidget();
	}
	
	return Reply;
}

FReply UG2ICutSceneWidget::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Reply = Super::NativeOnKeyUp(InGeometry, InKeyEvent);

	if (InKeyEvent.GetKey() == SkipKey)
	{
		bIsSkipPressed = false;
		SetSkipPercent(0.f);
	}
	
	return Reply;
}

void UG2ICutSceneWidget::SetSkipPercent(const float Percent) const
{
	if (!ensure(SkipBarMaterialInstance))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: SkipBarImage doesn't have material for circle bar"), *GetName());
		return;
	}
	
	float OutValue;
	if (!ensure(SkipBarMaterialInstance->GetScalarParameterValue(FName("Percent"), OutValue)))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: SkipBarImage's material for circle bar doesn't have Percent value"),
			*GetName());
		return;
	}
	
	SkipBarMaterialInstance->SetScalarParameterValue(FName("Percent"), Percent);
}

float UG2ICutSceneWidget::GetSkipPercent() const
{
	float OutValue = 0.f;
	if (!ensure(SkipBarMaterialInstance))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: SkipBarImage doesn't have material for circle bar"), *GetName());
		return OutValue;
	}

	SkipBarMaterialInstance->GetScalarParameterValue(FName("Percent"), OutValue);
	return OutValue;
}

void UG2ICutSceneWidget::CloseWidget() const
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->CloseCutScene(CurrentWidgetName);
}

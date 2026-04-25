#include "Menu/Options/G2IOptionsWidget.h"
#include "G2I.h"
#include "G2IUIManager.h"
#include "G2IWidgetNames.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"

void UG2IOptionsWidget::InitializeAfterManagerLoading()
{
	Super::InitializeAfterManagerLoading();

	InitializeDefaults();
	BindDelegates();
}

void UG2IOptionsWidget::InitializeDefaults() const
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->AddWidgetToPanel(OptionsSubWidgetSwitcher, EG2IWidgetNames::KeyboardAndMouseOptions);
	UIManager->AddWidgetToPanel(OptionsSubWidgetSwitcher, EG2IWidgetNames::ControlsOptions);
	UIManager->AddWidgetToPanel(OptionsSubWidgetSwitcher, EG2IWidgetNames::ScreenOptions);
	UIManager->AddWidgetToPanel(OptionsSubWidgetSwitcher, EG2IWidgetNames::GraphicsOptions);
	UIManager->AddWidgetToPanel(OptionsSubWidgetSwitcher, EG2IWidgetNames::SoundOptions);
}

void UG2IOptionsWidget::BindDelegates()
{
	if (ensure(BackButton))
	{
		BackButton->OnClicked.AddDynamic(this, &ThisClass::OnBackButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find BackButton"), *GetName());
	}

	if (ensure(ApplyButton))
	{
		ApplyButton->OnClicked.AddDynamic(this, &ThisClass::OnApplyButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find ApplyButton"), *GetName());
	}
	
	if (ensure(CancelButton))
	{
		CancelButton->OnClicked.AddDynamic(this, &ThisClass::OnCancelButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find CancelButton"), *GetName());
	}

	if (ensure(CommonOptionsButton))
	{
		CommonOptionsButton->OnClicked.AddDynamic(this, &ThisClass::OnCommonOptionsButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Button of Common Options"), *GetName());
	}
	if (ensure(ControlsOptionsButton))
	{
		ControlsOptionsButton->OnClicked.AddDynamic(this, &ThisClass::OnControlsOptionsButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Button of Controls Options"), *GetName());
	}
	if (ensure(ScreenOptionsButton))
	{
		ScreenOptionsButton->OnClicked.AddDynamic(this, &ThisClass::OnScreenOptionsButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Button of Screen Options"), *GetName());
	}
	if (ensure(GraphicsOptionsButton))
	{
		GraphicsOptionsButton->OnClicked.AddDynamic(this, &ThisClass::OnGraphicsOptionsButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Button of Graphics Options"), *GetName());
	}
	if (ensure(SoundOptionsButton))
	{
		SoundOptionsButton->OnClicked.AddDynamic(this, &ThisClass::OnSoundOptionsButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Button of Sound Options"), *GetName());
	}
}

void UG2IOptionsWidget::OnBackButtonClicked()
{
	if (ensure(OnBack))
	{
		OnBack();
	}
	else
	{
		UE_LOG(LogG2I, Warning, TEXT("Back function is undefined in %s"), *GetName());
	}
	
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->CancelAllUnAppliedOptions(OptionsSubWidgetSwitcher);
	UIManager->CloseWidget(EG2IWidgetNames::Options);
}

void UG2IOptionsWidget::OnApplyButtonClicked()
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->ApplyAllOptions(OptionsSubWidgetSwitcher);
}

void UG2IOptionsWidget::OnCancelButtonClicked()
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->CancelAllUnAppliedOptions(OptionsSubWidgetSwitcher);
}

void UG2IOptionsWidget::OnCommonOptionsButtonClicked()
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->SwitchWidget(OptionsSubWidgetSwitcher, EG2IWidgetNames::KeyboardAndMouseOptions);
}

void UG2IOptionsWidget::OnControlsOptionsButtonClicked()
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->SwitchWidget(OptionsSubWidgetSwitcher, EG2IWidgetNames::ControlsOptions);
}

void UG2IOptionsWidget::OnScreenOptionsButtonClicked()
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->SwitchWidget(OptionsSubWidgetSwitcher, EG2IWidgetNames::ScreenOptions);
}

void UG2IOptionsWidget::OnGraphicsOptionsButtonClicked()
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->SwitchWidget(OptionsSubWidgetSwitcher, EG2IWidgetNames::GraphicsOptions);
}

void UG2IOptionsWidget::OnSoundOptionsButtonClicked()
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->SwitchWidget(OptionsSubWidgetSwitcher, EG2IWidgetNames::SoundOptions);
}

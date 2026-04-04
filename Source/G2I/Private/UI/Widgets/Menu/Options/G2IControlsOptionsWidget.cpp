#include "Menu/Options/G2IControlsOptionsWidget.h"
#include "G2I.h"
#include "G2ICharacterDaughter.h"
#include "G2ICharacterEngineer.h"
#include "G2IUIManager.h"
#include "G2IWidgetNames.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Menu/Options/G2ICharacterControlsWidget.h"

void UG2IControlsOptionsWidget::InitializeAfterManagerLoading()
{
	Super::InitializeAfterManagerLoading();

	InitializeDefaults();
	BindDelegates();
}

void UG2IControlsOptionsWidget::InitializeDefaults()
{
	if (!ensure(CharactersSubWidgetSwitcher))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find WidgetSwitcher in Controls"), *GetName());
		return;
	}
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->SetupControlsWidget(CharactersSubWidgetSwitcher);
	OnEngineerControlsButtonClicked();
}

void UG2IControlsOptionsWidget::BindDelegates()
{
	if (ensure(EngineerControlButton))
	{
		EngineerControlButton->OnClicked.AddDynamic(this, &ThisClass::OnEngineerControlsButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Engineer's controls button"), *GetName());
	}
	if (ensure(DaughterControlButton))
	{
		DaughterControlButton->OnClicked.AddDynamic(this, &ThisClass::OnDaughterControlsButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Daughter's controls buttons"), *GetName());
	}
}

void UG2IControlsOptionsWidget::OnEngineerControlsButtonClicked()
{
	if (!ensure(CharactersSubWidgetSwitcher))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find WidgetSwitcher in Controls"), *GetName());
		return;
	}
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->SwitchWidget(CharactersSubWidgetSwitcher, EG2IWidgetNames::EngineerControlsOptions);
	if (UG2ICharacterControlsWidget *CharacterControlsWidget =
		Cast<UG2ICharacterControlsWidget>(CharactersSubWidgetSwitcher->GetActiveWidget()))
	{
		CharacterControlsWidget->Character = AG2ICharacterEngineer::StaticClass();
	}
	if (ensure(EngineerControlButton))
	{
		EngineerControlButton->SetIsEnabled(false);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Engineer's controls button"), *GetName());
	}
	if (ensure(DaughterControlButton))
	{
		DaughterControlButton->SetIsEnabled(true);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Daughter's controls buttons"), *GetName());
	}
}

void UG2IControlsOptionsWidget::OnDaughterControlsButtonClicked()
{
	if (!ensure(CharactersSubWidgetSwitcher))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find WidgetSwitcher in Controls"), *GetName());
		return;
	}
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->SwitchWidget(CharactersSubWidgetSwitcher, EG2IWidgetNames::DaughterControlsOptions);
	if (UG2ICharacterControlsWidget *CharacterControlsWidget =
		Cast<UG2ICharacterControlsWidget>(CharactersSubWidgetSwitcher->GetActiveWidget()))
	{
		CharacterControlsWidget->Character = AG2ICharacterDaughter::StaticClass();
	}
	if (ensure(EngineerControlButton))
	{
		EngineerControlButton->SetIsEnabled(true);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Engineer's controls button"), *GetName());
	}
	if (ensure(DaughterControlButton))
	{
		DaughterControlButton->SetIsEnabled(false);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Daughter's controls buttons"), *GetName());
	}
}
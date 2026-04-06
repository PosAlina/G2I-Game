#include "Menu/G2IPauseWidget.h"
#include "G2I.h"
#include "G2IGameInstance.h"
#include "G2IPlayerController.h"
#include "G2IUIManager.h"
#include "G2IWidgetNames.h"
#include "Components/Button.h"

void UG2IPauseWidget::InitializeAfterManagerLoading()
{
	Super::InitializeAfterManagerLoading();

	BindDelegates();
}

void UG2IPauseWidget::BindDelegates()
{
	if (ensure(ContinueButton))
	{
		ContinueButton->OnClicked.AddDynamic(this, &ThisClass::OnContinueButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find ContinueButton"), *GetName());
	}
	if (ensure(OptionsButton))
	{
		OptionsButton->OnClicked.AddDynamic(this, &ThisClass::OnOptionsButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find OptionsButton"), *GetName());
	}
	if (ensure(MainMenuButton))
	{
		MainMenuButton->OnClicked.AddDynamic(this, &ThisClass::OnMainMenuButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find MainMenuButton"), *GetName());
	}
}

void UG2IPauseWidget::OnContinueButtonClicked()
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->CloseWidget(EG2IWidgetNames::Pause);

	if (OnContinue)
	{
		OnContinue();
	}
}

void UG2IPauseWidget::OnOptionsButtonClicked()
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	
	UIManager->HideWidget(EG2IWidgetNames::Pause);
	UIManager->OpenWidget(EG2IWidgetNames::Options);
	
	UIManager->SetupOptionsWidget(GetShowCurrentWidgetFunction());
}

void UG2IPauseWidget::OnMainMenuButtonClicked()
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	
	UIManager->OpenWidget(EG2IWidgetNames::Confirmation);
	UIManager->SetupConfirmationWidget([this]()
	{
		if (!ensure(GameInstance))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
				*UG2IGameInstance::StaticClass()->GetName());
			return;
		}
		GameInstance->LoadMainMenuLevel();
	},
	GetShowCurrentWidgetFunction(),
	"Confirmation.Question.OpenMainMenu",
	"Confirmation.Confirm.OpenMainMenu",
	"Confirmation.Cancel.OpenMainMenu");
}

TFunction<void()> UG2IPauseWidget::GetShowCurrentWidgetFunction() const
{
	TFunction<void()> ShowCurrentWidget = [this]()
	{
		if (!ensure(UIManager))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
				*UG2IUIManager::StaticClass()->GetName());
			return;
		}
		UIManager->ShowWidget(EG2IWidgetNames::Pause);
	};
	return ShowCurrentWidget;
}

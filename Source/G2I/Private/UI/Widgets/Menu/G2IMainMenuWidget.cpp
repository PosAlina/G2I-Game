#include "Menu/G2IMainMenuWidget.h"
#include "G2I.h"
#include "G2IGameInstance.h"
#include "G2IPlayerController.h"
#include "G2IUIManager.h"
#include "G2IWidgetNames.h"
#include "Components/Button.h"

void UG2IMainMenuWidget::InitializeAfterManagerLoading()
{
	Super::InitializeAfterManagerLoading();

	InitializeDefaults();
	BindDelegates();
}

void UG2IMainMenuWidget::InitializeDefaults() const
{
	if (ensure(ContinueButton))
	{
		ContinueButton->SetIsEnabled(false);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("Continue button isn't existed in %s"), *GetName());
	}
}

void UG2IMainMenuWidget::BindDelegates()
{
	if (ensure(NewGameButton))
	{
		NewGameButton->OnClicked.AddDynamic(this, &ThisClass::OnNewGameButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("New game button isn't existed in %s"), *GetName());
	}
	if (ensure(OptionsButton))
	{
		OptionsButton->OnClicked.AddDynamic(this, &ThisClass::OnOptionsButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("Options button isn't existed in %s"), *GetName());
	}
	if (ensure(CreatorsButton))
	{
		CreatorsButton->SetIsEnabled(false);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("Creators button isn't existed in %s"), *GetName());
	}
	if (ensure(QuitGameButton))
	{
		QuitGameButton->OnClicked.AddDynamic(this, &ThisClass::OnQuitGameButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("Quit game button isn't existed in %s"), *GetName());
	}
}

void UG2IMainMenuWidget::OnNewGameButtonClicked()
{
	LoadNewGame();

	// TODO: Add correlation from saving system
	// if Saves exists:
	// NewGameWithSaveExists()
}

void UG2IMainMenuWidget::LoadNewGame() const
{
	if (!ensure(GameInstance))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IGameInstance::StaticClass()->GetName());
		return;
	}
	if (!GameInstance->LoadLevel(0))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Failed to start new game"), *GetName());
	}
}

void UG2IMainMenuWidget::NewGameWithSaveExists() const
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}

	UIManager->HideWidget(EG2IWidgetNames::MainMenu);
	UIManager->OpenWidget(EG2IWidgetNames::Confirmation);

	UIManager->SetupConfirmationWidget([this]()
	{
		// TODO: Add removing saves
		LoadNewGame();
	},
	GetShowCurrentWidgetFunction(),
	"Confirmation.Question.NewGame",
	"Confirmation.Confirm.NewGame",
	"Confirmation.Cancel.NewGame");
}

void UG2IMainMenuWidget::OnOptionsButtonClicked()
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->HideWidget(EG2IWidgetNames::MainMenu);
	UIManager->OpenWidget(EG2IWidgetNames::Options);
	
	UIManager->SetupOptionsWidget(GetShowCurrentWidgetFunction());
}

void UG2IMainMenuWidget::OnQuitGameButtonClicked()
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	
	UIManager->HideWidget(EG2IWidgetNames::MainMenu);
	UIManager->OpenWidget(EG2IWidgetNames::Confirmation);
	
	UIManager->SetupConfirmationWidget([this]()
	{
		if (!ensure(PlayerController))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
				*AG2IPlayerController::StaticClass()->GetName());
			return;
		}
		PlayerController->QuitGame();
	},
	GetShowCurrentWidgetFunction(),
	"Confirmation.Question.QuitGame",
	"Confirmation.Confirm.QuitGame",
	"Confirmation.Cancel.QuitGame");
}

TFunction<void()> UG2IMainMenuWidget::GetShowCurrentWidgetFunction() const
{
	TFunction<void()> ShowCurrentWidget = [this]()
	{
		if (!ensure(UIManager))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
				*UG2IUIManager::StaticClass()->GetName());
			return;
		}
		UIManager->ShowWidget(EG2IWidgetNames::MainMenu);
	};
	return ShowCurrentWidget;
}

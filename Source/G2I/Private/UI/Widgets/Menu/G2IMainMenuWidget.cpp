#include "Menu/G2IMainMenuWidget.h"
#include "G2I.h"
#include "G2IGameInstance.h"
#include "G2IPlayerController.h"
#include "G2IUIManager.h"
#include "G2IWidgetNames.h"
#include "Components/Button.h"
#include "G2ISavingGameplayManager.h"

void UG2IMainMenuWidget::InitializeAfterManagerLoading()
{
	Super::InitializeAfterManagerLoading();

	if (!ensure(GameInstance))
	{
		UE_LOG(LogG2I, Error, TEXT("GameInstance is null %s"), *GetName());
		return;
	}

	SaveManager = GameInstance->GetSubsystem<UG2ISavingGameplayManager>();

	InitializeDefaults();

	if (SaveManager)
	{
		const bool bHasSave = SaveManager->DoesSaveExist();
		if (ContinueButton)
		{
			ContinueButton->SetIsEnabled(bHasSave);
		}
	}

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
		CreatorsButton->OnClicked.AddDynamic(this, &ThisClass::OnCreatorsButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("Creators button isn't existed in %s"), *GetName());
	}
	if (ensure(GalleryButton))
	{
		GalleryButton->OnClicked.AddDynamic(this, &ThisClass::OnGalleryButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("Gallery button isn't existed in %s"), *GetName());
	}
	if (ensure(QuitGameButton))
	{
		QuitGameButton->OnClicked.AddDynamic(this, &ThisClass::OnQuitGameButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("Quit game button isn't existed in %s"), *GetName());
	}
	if (ensure(ContinueButton))
	{
		ContinueButton->OnClicked.AddDynamic(this, &ThisClass::OnContinueButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("Continue button isn't existed in %s"), *GetName());
	}
}

void UG2IMainMenuWidget::OnNewGameButtonClicked()
{
	if (!ensure(SaveManager))
	{
		UE_LOG(LogG2I, Error, TEXT("SaveManager is null %s"), *GetName());
		return;
	}

	if (SaveManager->DoesSaveExist())
	{
		NewGameWithSaveExists();
	}
	else
	{
		LoadNewGame();
	}
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
		if (SaveManager)
		{
			SaveManager->ResetProgress();
		}
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

void UG2IMainMenuWidget::OnCreatorsButtonClicked()
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->HideWidget(EG2IWidgetNames::MainMenu);
	UIManager->OpenWidget(EG2IWidgetNames::Creators);

	UIManager->SetupCreatorsWidget(GetShowCurrentWidgetFunction());
}

void UG2IMainMenuWidget::OnGalleryButtonClicked()
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->HideWidget(EG2IWidgetNames::MainMenu);
	UIManager->OpenWidget(EG2IWidgetNames::Gallery);
	
	UIManager->SetupGalleryWidget(GetShowCurrentWidgetFunction());
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

void UG2IMainMenuWidget::OnContinueButtonClicked()
{

	if (!ensure(SaveManager))
	{
		UE_LOG(LogG2I, Error, TEXT("SaveManager is null %s"), *GetName());
		return;
	}

	SaveManager->LoadGameplay(false);

	EG2ILevelName SavedLevel = SaveManager->LoadCurrentLevel();

	if (SavedLevel == EG2ILevelName::None)
	{
		UE_LOG(LogG2I, Warning, TEXT("Saved level is None"));
		return;
	}

	if (!ensure(GameInstance))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IGameInstance::StaticClass()->GetName());
		return;
	}
	GameInstance->LoadLevel(SavedLevel);
}

void UG2IMainMenuWidget::StartLevelInitialize()
{
	Super::StartLevelInitialize();

	if (!ensure(GameInstance))
	{
		UE_LOG(LogG2I, Error, TEXT("GameInstance is null %s"), *GetName());
		return;
	}

	if (!ensure(SaveManager))
	{
		UE_LOG(LogG2I, Error, TEXT("SaveManager is null %s"), *GetName());
		return;
	}

	if (ContinueButton)
	{
		ContinueButton->SetIsEnabled(SaveManager->DoesSaveExist());
	}
}
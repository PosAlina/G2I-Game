#include "Menu/G2IPauseWidget.h"
#include "G2I.h"
#include "G2IPlayerController.h"
#include "G2IUIManager.h"
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
		OptionsButton->SetIsEnabled(false);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find OptionsButton"), *GetName());
	}
	if (ensure(MainMenuButton))
	{
		MainMenuButton->SetIsEnabled(false);
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
		UE_LOG(LogG2I, Warning, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*AG2IPlayerController::StaticClass()->GetName());
		return;
	}
	
	UIManager->CloseUI();

	PlayerController->SetInputMode(FInputModeGameOnly());
	PlayerController->bShowMouseCursor = false;
	PlayerController->SetPause(false);
}

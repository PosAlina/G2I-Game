#include "G2IUIInputHandler.h"
#include "G2I.h"
#include "G2IPlayerController.h"
#include "G2IUIDisplayManager.h"
#include "G2IWidgetsEnums.h"
#include "Components/Button.h"

void UG2IUIInputHandler::Initialize(UG2IUIManager *InUIManager, UG2IUIDisplayManager* InDisplayManager)
{
	UIManager = InUIManager;
	DisplayManager = InDisplayManager;

	if (GetWorld())
	{
		GetWorld()->OnWorldBeginPlay.AddUObject(this, &ThisClass::SetupDefaults);
	}
}

void UG2IUIInputHandler::SetupDefaults()
{
	const UWorld *World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}
	APlayerController *LocalPlayerController = World->GetFirstPlayerController();
	if (!ensure(LocalPlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Local player controller doesn't exist in %s"), *GetName());
		return;
	}

	PlayerController = Cast<AG2IPlayerController>(LocalPlayerController);
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Player Controller doesn't exist in %s"), *GetName());
		return;
	}

	BindDelegates();
}

void UG2IUIInputHandler::BindDelegates()
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
		return;
	}

	if (UButton *ContinueButton = DisplayManager->GetUIElement<UButton>(EG2IWidgetNames::Pause, "ContinueButton"))
	{
		ContinueButton->OnClicked.AddDynamic(this, &ThisClass::ContinueGame);
	}
	if (UButton *ExitGameButton = DisplayManager->GetUIElement<UButton>(EG2IWidgetNames::Pause, "QuitGameButton"))
	{
		ExitGameButton->OnClicked.AddDynamic(this, &ThisClass::QuitGame);
	}
}

void UG2IUIInputHandler::ContinueGame()
{
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("PlayerController doesn't exist in %s"), *GetName());
		return;
	}

	PlayerController->SetInputMode(FInputModeGameOnly());
	PlayerController->bShowMouseCursor = false;
	PlayerController->SetPause(false);
}

void UG2IUIInputHandler::QuitGame()
{
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("PlayerController doesn't exist in %s"), *GetName());
		return;
	}
	
	PlayerController->QuitGame();
}

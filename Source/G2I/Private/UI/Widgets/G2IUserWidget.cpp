#include "G2IUserWidget.h"
#include "G2I.h"
#include "G2IGameInstance.h"
#include "G2IPlayerController.h"
#include "G2IUIManager.h"

void UG2IUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	World = GetWorld();
	InitializeGameInstance();
	InitializePlayerController();
	InitializeUIManager();
}

void UG2IUserWidget::InitializeUIManager()
{
	if (!ensure(GameInstance))
	{
		UE_LOG(LogG2I, Error, TEXT("Game Instance doesn't exist in %s"), *GetName());
		return;
	}
	UIManager = GameInstance->GetSubsystem<UG2IUIManager>();
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Warning, TEXT("Couldn't get %s subsystem from GameInstance in %s"), *UG2IUIManager::StaticClass()->GetName(),
			*GetName());
	}
	UIManager->OnUIManagerInitialized.AddDynamic(this, &ThisClass::InitializeAfterManagerLoading);
}

void UG2IUserWidget::InitializePlayerController()
{
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}
	
	APlayerController *LocalPlayerController = World->GetFirstPlayerController();
	if (!ensure(LocalPlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Player controller doesn't exist in %s"), *GetName());
		return;
	}

	PlayerController = Cast<AG2IPlayerController>(LocalPlayerController);
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Player Controller isn't %s in %s"),
			*AG2IPlayerController::StaticClass()->GetName(), *GetName());
	}
}

void UG2IUserWidget::InitializeGameInstance()
{
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}
	GameInstance = Cast<UG2IGameInstance>(World->GetGameInstance());
	if (!ensure(GameInstance))
	{
		UE_LOG(LogG2I, Error, TEXT("Game Instance isn't %s in %s"),
			*UG2IGameInstance::StaticClass()->GetName(), *GetName());
	}
}

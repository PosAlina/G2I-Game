#include "G2IUIManager.h"
#include "G2I.h"
#include "G2IGameInstance.h"
#include "G2IPlayerController.h"
#include "G2IUIDisplayManager.h"
#include "G2IUIInputHandler.h"
#include "G2IWidgetsEnums.h"
#include "G2IWidgetsCatalog.h"
#include "G2IWorldHintKeyWidgetComponent.h"

void UG2IUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	OnPlayerControllerInitDelegate.AddDynamic(this, &ThisClass::InitializeComponents);
}

void UG2IUIManager::InitializeComponents(APlayerController* InPlayerController)
{
	PlayerController = Cast<AG2IPlayerController>(InPlayerController);
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("PlayerController doesn't exist in %s"), *GetName());
		return;
	}
	PlayerController->OnPossessPawnDelegate.AddDynamic(this, &ThisClass::SetMechanicsDescriptionByCharacter);
	
	GameInstance = Cast<UG2IGameInstance>(GetGameInstance());
	if (!ensure(GameInstance))
	{
		UE_LOG(LogG2I, Error, TEXT("Game Instance isn't %s in %s"),
			*UG2IGameInstance::StaticClass()->GetName(), *GetName());
		return;
	}
	WidgetsCatalog = GameInstance->WidgetsCatalog;
	if (!ensure(WidgetsCatalog))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"), *UG2IWidgetsCatalog::StaticClass()->GetName(), *GetName());
		return;
	}

	DisplayManager = NewObject<UG2IUIDisplayManager>(this);
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
		return;
	}
	DisplayManager->Initialize(WidgetsCatalog);

	UIHandler = NewObject<UG2IUIInputHandler>(this);
	if (!ensure(UIHandler))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
			*UG2IUIInputHandler::StaticClass()->GetName(), *GetName());
		return;
	}
	UIHandler->Initialize(this, DisplayManager);

	InitializeDefaultsWidgets();
	OpenHUD();
}

void UG2IUIManager::InitializeDefaultsWidgets()
{
	InitializeAimingWidget();
	InitializeTrainingScreenWidget();
}

void UG2IUIManager::OpenHUD() const
{
	OpenTrainingWidget();
}

void UG2IUIManager::OpenWorldWidget(UG2IWorldHintWidgetComponent* WidgetComponent) const
{
	if (!ensure(WidgetComponent))
	{
		UE_LOG(LogG2I, Warning, TEXT("Attempting to open nullptr widget component in %s"), *GetName());
		return;
	}
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
		return;
	}

	DisplayManager->ShowWorldWidget(*WidgetComponent);
}

void UG2IUIManager::CloseWorldWidget(UG2IWorldHintWidgetComponent* WidgetComponent) const
{
	if (!ensure(WidgetComponent))
	{
		UE_LOG(LogG2I, Warning, TEXT("Attempting to close nullptr widget component in %s"), *GetName());
		return;
	}
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
		return;
	}

	DisplayManager->HideWorldWidget(*WidgetComponent);
}

bool UG2IUIManager::CanSeeWorldWidget(UG2IWorldHintWidgetComponent *WidgetComponent) const
{
	if (!ensure(WidgetComponent))
	{
		UE_LOG(LogG2I, Warning, TEXT("Attempting to check visibility of nullptr widget component in %s"),
			*GetName());
		return false;
	}
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
		return false;
	}

	return DisplayManager->IsVisibleWorldWidget(*WidgetComponent);
}

UG2IUserWidget* UG2IUIManager::CreateWidgetByName(const EG2IWidgetNames WidgetName) const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
		return nullptr;
	}

	return DisplayManager->CreateNewWidget(WidgetName);
}

void UG2IUIManager::InitializeAimingWidget() const
{
	if (!ensure(WidgetsCatalog))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"), *UG2IWidgetsCatalog::StaticClass()->GetName(), *GetName());
		return;
	}
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
		return;
	}
	
	UTexture2D *NewAimTexture = WidgetsCatalog->AimingViewTypes[EG2IAimType::DefaultAim];
	DisplayManager->SetImage(EG2IWidgetNames::Aim, "AimImage", NewAimTexture);
}

void UG2IUIManager::OpenAimingWidget() const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
		return;
	}
	
	DisplayManager->ShowWidget(EG2IWidgetNames::Aim);
}

void UG2IUIManager::CloseAimingWidget() const
{
	if (!ensure(WidgetsCatalog))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"), *UG2IWidgetsCatalog::StaticClass()->GetName(), *GetName());
		return;
	}
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
		return;
	}
	
	DisplayManager->HideWidget(EG2IWidgetNames::Aim);
}

void UG2IUIManager::ChangeAimingType(const EG2IAimType NewAimType) const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
		return;
	}

	UTexture2D *NewAimTexture = WidgetsCatalog->AimingViewTypes[NewAimType];
	DisplayManager->SetImage(EG2IWidgetNames::Aim, "AimImage", NewAimTexture);
}

void UG2IUIManager::OpenPauseWidget() const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
		return;
	}
	
	DisplayManager->ShowWidget(EG2IWidgetNames::Pause);
}

void UG2IUIManager::ClosePauseWidget() const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
		return;
	}
	
	DisplayManager->HideWidget(EG2IWidgetNames::Pause);
}

void UG2IUIManager::InitializeTrainingScreenWidget()
{
	SetCommonMechanicsDescription();

	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("PlayerController doesn't exist in %s"), *GetName());
		return;
	}
	APawn *Pawn = PlayerController->GetPawn();
	SetMechanicsDescriptionByCharacter(Pawn);
}

void UG2IUIManager::OpenTrainingWidget() const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
		return;
	}
	
	DisplayManager->ShowWidget(EG2IWidgetNames::TrainingScreen);
}

void UG2IUIManager::SetCommonMechanicsDescription() const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
		return;
	}
	if (!ensure(WidgetsCatalog))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"), *UG2IWidgetsCatalog::StaticClass()->GetName(), *GetName());
		return;
	}

	FString Description;
	for (auto Text : WidgetsCatalog->CommonMechanicsDescription.NextStrings)
	{
		Description += Text + "\n";
	}
	DisplayManager->SetText(EG2IWidgetNames::TrainingScreen, "CommonDescriptionTextBlock", Description);
}

void UG2IUIManager::SetMechanicsDescriptionByCharacter(APawn *Pawn)
{
	if (!Pawn)
	{
		return;
	}
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
		return;
	}
	if (!ensure(WidgetsCatalog))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"), *UG2IWidgetsCatalog::StaticClass()->GetName(), *GetName());
		return;
	}

	FString Description;
	if (FG2IMultiText *Descriptions = WidgetsCatalog->CharactersMechanicsDescription.Find(Pawn->GetClass()))
	{
		for (FString Text : Descriptions->NextStrings)
		{
			Description += Text + "\n";
		}
	}
	DisplayManager->SetText(EG2IWidgetNames::TrainingScreen, "CharacterDescriptionTextBlock", Description);
}

void UG2IUIManager::SetKeyByInputAction(UG2IWorldHintWidgetComponent* WidgetComponent, UInputAction* InputAction) const
{
	if (!ensure(InputAction))
	{
		UE_LOG(LogG2I, Warning, TEXT("Attempting to set key of undefined input action in widget in %s"),
			*GetName());
		return;
	}
	if (!ensure(WidgetComponent))
	{
		UE_LOG(LogG2I, Warning, TEXT("Attempting to set key for %s in nullptr widget component in %s"),
			*InputAction->GetName(), *GetName());
		return;
	}
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
		return;
	}
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("PlayerController doesn't exist in %s"), *GetName());
		return;
	}

	UG2IUserWidget *Widget = WidgetComponent->FindOrAddWidgetByName(EG2IWidgetNames::KeyHint);
	const FName Key = PlayerController->GetKeyName(InputAction);
	DisplayManager->SetText(Widget, "KeyTextBlock", Key.ToString());
}

void UG2IUIManager::SetKeyWidgetSize(UG2IWorldHintKeyWidgetComponent* WidgetComponent) const
{
	if (!ensure(WidgetComponent))
	{
		UE_LOG(LogG2I, Warning, TEXT("Attempting to set key in nullptr widget component in %s"), *GetName());
		return;
	}
	if (!ensure(WidgetsCatalog))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"), *UG2IWidgetsCatalog::StaticClass()->GetName(), *GetName());
		return;
	}

	WidgetComponent->SetWidgetSize(WidgetsCatalog->KeyWidgetDefaultSize);
}

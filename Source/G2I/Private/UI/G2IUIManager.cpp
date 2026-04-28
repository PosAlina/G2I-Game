#include "G2IUIManager.h"
#include "G2I.h"
#include "G2IAimTypeEnum.h"
#include "G2ICharacterDaughter.h"
#include "G2ICharacterEngineer.h"
#include "G2IConfirmationWidget.h"
#include "G2ICutScenesParameters.h"
#include "G2IGameInstance.h"
#include "G2IPlayerController.h"
#include "G2IStringTablesTypes.h"
#include "G2IUIDisplayManager.h"
#include "G2IUpdateOptions.h"
#include "G2IWidgetComponentParameters.h"
#include "G2IWidgetNames.h"
#include "G2IWorldHintKeyWidgetComponent.h"
#include "Components/ListView.h"
#include "Components/RichTextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Gameplay/G2IKeyHintWidget.h"
#include "HUD/G2IAimingWidget.h"
#include "Menu/G2ICreatorsWidget.h"
#include "Menu/G2IPauseWidget.h"
#include "Menu/Elements/NumericalRow/G2INumericalMultiValuePropertyRow.h"
#include "Menu/Elements/G2IControlListItem.h"
#include "Menu/Elements/G2IControlRow.h"
#include "Menu/Elements/TextRow/G2ITextMultiValuePropertyRow.h"
#include "Menu/Gallery/G2IGalleryWidget.h"
#include "Menu/Options/G2ICharacterControlsWidget.h"
#include "Menu/Options/G2IOptionsWidget.h"

void UG2IUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	GameInstance = Cast<UG2IGameInstance>(GetGameInstance());
	if (!ensure(GameInstance))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IGameInstance::StaticClass()->GetName());
		return;
	}
	StartGameDelegateHandle = GameInstance->OnStartLevelInitDelegate.AddUObject(this, &ThisClass::InitializeInStartGame);
}

void UG2IUIManager::InitializeInStartGame()
{
	if (!ensure(GameInstance))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IGameInstance::StaticClass()->GetName());
		return;
	}
	GameInstance->OnStartLevelInitDelegate.Remove(StartGameDelegateHandle);
	
	InitializeDefaultsInStartGame();
	InitializeDefaultsInStartLevel();
	PostInitializeDefaultsInStartGame();
	PostInitializeDefaultsInStartLevel();

	GameInstance->OnCloseLevelDelegate.AddUObject(this, &ThisClass::CloseLevelUI);
	GameInstance->OnStartLevelInitDelegate.AddUObject(this, &ThisClass::InitializeInStartLevel);
	
	OnUIManagerInitialized.Broadcast();
}

void UG2IUIManager::InitializeInStartLevel()
{
	InitializeDefaultsInStartLevel();
	PostInitializeDefaultsInStartLevel();
}

void UG2IUIManager::InitializeDefaultsInStartGame()
{
	CutScenesParameters = GameInstance->GetCutScenesParameters();
	if (!ensure(CutScenesParameters))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
			*GetName(), *UG2ICutScenesParameters::StaticClass()->GetName());
	}

	DisplayManager = NewObject<UG2IUIDisplayManager>(this);
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIDisplayManager::StaticClass()->GetName());
		return;
	}
}

void UG2IUIManager::InitializeDefaultsInStartLevel()
{
	const UWorld *World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World is null in %s"), *GetName());
		return;
	}
	
	PlayerController = Cast<AG2IPlayerController>(World->GetFirstPlayerController());
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*AG2IPlayerController::StaticClass()->GetName());
		return;
	}
	
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIDisplayManager::StaticClass()->GetName());
		return;
	}
	DisplayManager->InitializeInStartLevel();
}

void UG2IUIManager::PostInitializeDefaultsInStartGame() const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIDisplayManager::StaticClass()->GetName());
		return;
	}
	DisplayManager->PostInitializeInStartGame();
}

void UG2IUIManager::PostInitializeDefaultsInStartLevel() const
{
	InitializeNewLevelUI();
}

void UG2IUIManager::InitializeNewLevelUI() const
{
	if (!ensure(GameInstance))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IGameInstance::StaticClass()->GetName());
		return;
	}
	
	if (GameInstance->IsMainMenuLevel())
	{
		OpenWidget(EG2IWidgetNames::MainMenu);
		return;
	}

	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIDisplayManager::StaticClass()->GetName());
		OpenHUD();
		return;
	}
	
	const EG2ILevelName LevelName = GameInstance->GetCurrentLevelEnum();

#if WITH_EDITOR
	if (!ensure(CutScenesParameters))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
			*GetName(), *UG2ICutScenesParameters::StaticClass()->GetName());
	}
	if (!CutScenesParameters->bIsOnInEditor)
	{
		OpenHUD();
		return;
	}
#endif
	
	if (LevelName == EG2ILevelName::BoilerRoom)
	{
		if (DisplayManager->GetWidget(EG2IWidgetNames::CutSceneStartBoilerRoom))
		{
			OpenWidget(EG2IWidgetNames::CutSceneStartBoilerRoom);
			return;
		}
		OpenHUD();
		return;
	}

	// Other levels
	OpenHUD();
}

void UG2IUIManager::CloseCutScene(const EG2IWidgetNames WidgetName) const
{
	CloseWidget(WidgetName);
	if (WidgetName == EG2IWidgetNames::CutSceneEndGame)
	{
		if (!ensure(GameInstance))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
				*UG2IGameInstance::StaticClass()->GetName());
			return;
		}
		GameInstance->LoadMainMenuLevel();
		return;
	}

	OpenHUD();
}

void UG2IUIManager::CloseLevelUI() const
{
	CloseAllWidgets();
	ShowAllWidgets(); // For visibility widgets in next level
}

FString UG2IUIManager::GetWidgetNameString(EG2IWidgetNames WidgetName) const
{
	const UEnum* WidgetNamesEnumPtr = StaticEnum<EG2IWidgetNames>();
	if (!ensure(WidgetNamesEnumPtr))
	{
		UE_LOG(LogG2I, Warning, TEXT("Couldn't find enum WidgetNames in %s"),*GetName());
		return "";
	}

	return WidgetNamesEnumPtr->GetNameStringByValue(static_cast<int64>(WidgetName));
}

void UG2IUIManager::OpenHUD() const
{
	PlayerController->SetInputMode(FInputModeGameOnly());
	PlayerController->bShowMouseCursor = false;
	PlayerController->SetPause(false);
	
	OpenWidget(EG2IWidgetNames::TrainingScreen, false);
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
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIDisplayManager::StaticClass()->GetName());
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
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIDisplayManager::StaticClass()->GetName());
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
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIDisplayManager::StaticClass()->GetName());
		return false;
	}

	return DisplayManager->IsVisibleWorldWidget(*WidgetComponent);
}

UG2IUserWidget* UG2IUIManager::CreateWidgetByName(const EG2IWidgetNames WidgetName) const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIDisplayManager::StaticClass()->GetName());
		return nullptr;
	}

	return DisplayManager->CreateNewWidget(WidgetName);
}

void UG2IUIManager::AddWidgetToPanel(UPanelWidget* Panel, const EG2IWidgetNames WidgetName) const
{
	if (!ensure(Panel))
	{
		UE_LOG(LogG2I, Warning, TEXT("An attempt to change nullptr %s in %s"),
			*UPanelWidget::StaticClass()->GetName(), *GetName());
		return;
	}
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIDisplayManager::StaticClass()->GetName());
		return;
	}
	
	UG2IUserWidget *Widget = DisplayManager->GetWidget(WidgetName);
	if (!ensure(Widget))
	{
		UE_LOG(LogG2I, Warning, TEXT("Widget %s doesn't exist in %s"), *GetWidgetNameString(WidgetName),
			*GetName());
		return;
	}

	Panel->AddChild(Widget);
}

void UG2IUIManager::SwitchWidget(UWidgetSwitcher* Switcher, const EG2IWidgetNames WidgetName) const
{
	if (!ensure(Switcher))
	{
		UE_LOG(LogG2I, Warning, TEXT("An attempt to change nullptr %s in %s"),
			*UWidgetSwitcher::StaticClass()->GetName(), *GetName());
		return;
	}
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIDisplayManager::StaticClass()->GetName());
		return;
	}
	UG2IUserWidget *Widget = DisplayManager->GetWidget(WidgetName);
	if (!ensure(Widget))
	{
		UE_LOG(LogG2I, Warning, TEXT("Widget %s doesn't exist in %s"),
			*GetWidgetNameString(WidgetName), *GetName());
		return;
	}

	Switcher->SetActiveWidget(Widget);
}

void UG2IUIManager::OpenWidget(const EG2IWidgetNames WidgetName, const bool bIsFocus) const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIDisplayManager::StaticClass()->GetName());
		return;
	}
	
	DisplayManager->OpenWidget(WidgetName, bIsFocus);
}

void UG2IUIManager::CloseWidget(const EG2IWidgetNames WidgetName) const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIDisplayManager::StaticClass()->GetName());
		return;
	}
	
	DisplayManager->CloseWidget(WidgetName);
}

void UG2IUIManager::ShowWidget(const EG2IWidgetNames WidgetName) const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIDisplayManager::StaticClass()->GetName());
		return;
	}
	DisplayManager->ShowWidget(WidgetName);
}

void UG2IUIManager::HideWidget(const EG2IWidgetNames WidgetName) const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIDisplayManager::StaticClass()->GetName());
		return;
	}
	DisplayManager->HideWidget(WidgetName);
}

void UG2IUIManager::ShowAllWidgets() const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIDisplayManager::StaticClass()->GetName());
		return;
	}

	DisplayManager->ShowAllHiddenWidgets();
}

void UG2IUIManager::CloseAllWidgets() const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIDisplayManager::StaticClass()->GetName());
		return;
	}

	DisplayManager->CloseAllActiveWidgets();
}

void UG2IUIManager::CloseUI() const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIDisplayManager::StaticClass()->GetName());
		return;
	}

	DisplayManager->CloseActiveWidgetsByType(EG2IWidgetTypes::UI);
}

void UG2IUIManager::ChangeAimingType(const EG2IAimType NewAimType) const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
		return;
	}

	UG2IAimingWidget *Widget = Cast<UG2IAimingWidget>(DisplayManager->GetWidget(EG2IWidgetNames::Aim));
	if (!ensure(Widget))
	{
		UE_LOG(LogG2I, Warning, TEXT("Aiming widget doesn't exist in %s"), *GetName());
		return;
	}
	
	Widget->SetAimingViewType(NewAimType);
}

void UG2IUIManager::SetKeyByInputAction(UG2IWorldHintWidgetComponent* WidgetComponent, UInputAction* InputAction,
                                        const TSubclassOf<APawn>& PawnClass) const
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
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("PlayerController doesn't exist in %s"), *GetName());
		return;
	}
	if (!ensure(PawnClass))
	{
		UE_LOG(LogG2I, Warning, TEXT("Attempting to set key of null character in widget in %s"), *GetName());
		return;
	}

	if (const UG2IKeyHintWidget *Widget =
		Cast<UG2IKeyHintWidget>(WidgetComponent->FindOrAddWidgetByName(EG2IWidgetNames::KeyHint)))
	{
		if (!ensure(Widget->KeyTextBlock))
		{
			UE_LOG(LogG2I, Warning, TEXT("An attempt to change nullptr key text block of %s in %s"),
				*Widget->GetName(), *GetName());
			return;
		}
		const FName Key = PlayerController->GetKeyName(InputAction, PawnClass);
		Widget->KeyTextBlock->SetText(FText::FromName(Key));
	}
}

void UG2IUIManager::SetupConfirmationWidget(const TFunction<void()>& NewConfirmAction,
	const TFunction<void()>& NewCancelAction, const FString& NewQuestionStringID,
                                            const FString& NewConfirmStringID, const FString& NewCancelStringID) const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIDisplayManager::StaticClass()->GetName());
		return;
	}
	if (UG2IConfirmationWidget *Widget = Cast<UG2IConfirmationWidget>(
		DisplayManager->GetWidget(EG2IWidgetNames::Confirmation)))
	{
		Widget->OnConfirm = NewConfirmAction;
		Widget->OnCancel = NewCancelAction;
		
		DisplayManager->SetText<URichTextBlock>(Widget->QuestionTextBlock, EG2IStringTablesTypes::Confirmations,
			NewQuestionStringID, "Confirmation.Question");
		DisplayManager->SetText<URichTextBlock>(Widget->ConfirmTextBlock, EG2IStringTablesTypes::Confirmations,
			NewConfirmStringID, "Confirmation.Confirm");
		DisplayManager->SetText<URichTextBlock>(Widget->CancelTextBlock, EG2IStringTablesTypes::Confirmations,
			NewCancelStringID, "Confirmation.Cancel");
	}
}

void UG2IUIManager::SetupOptionsWidget(const TFunction<void()>& NewBackAction) const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIDisplayManager::StaticClass()->GetName());
		return;
	}
	if (UG2IOptionsWidget *Widget = Cast<UG2IOptionsWidget>(
		DisplayManager->GetWidget(EG2IWidgetNames::Options)))
	{
		Widget->OnBack = NewBackAction;
	}
}

void UG2IUIManager::SetupCreatorsWidget(const TFunction<void()>& NewBackAction) const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIDisplayManager::StaticClass()->GetName());
		return;
	}
	if (UG2ICreatorsWidget *Widget = Cast<UG2ICreatorsWidget>(
		DisplayManager->GetWidget(EG2IWidgetNames::Creators)))
	{
		Widget->OnBack = NewBackAction;
	}
}

void UG2IUIManager::SetPropertyRow(UG2ITextMultiValuePropertyRow* PropertySelector, const FString& PropertyNameStringID,
                                   TArray<FString>& ValuesNamesStringID, const int32 DefaultValueIndex) const
{
	if (!ensure(PropertySelector))
	{
		UE_LOG(LogG2I, Warning, TEXT("An attempt to change nullptr %s in %s"),
			*UG2ITextMultiValuePropertyRow::StaticClass()->GetName(), *GetName());
		return;
	}
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIDisplayManager::StaticClass()->GetName());
		return;
	}
	
	DisplayManager->SetText<URichTextBlock>(PropertySelector->PropertyName, EG2IStringTablesTypes::Options,
		PropertyNameStringID, "PropertyName");
	for (FString& ValueStringID : ValuesNamesStringID)
	{
		URichTextBlock *ValueTextBlock = NewObject<URichTextBlock>();
		DisplayManager->SetText<URichTextBlock>(ValueTextBlock, EG2IStringTablesTypes::Options,
			ValueStringID, "PropertyValue");
		FText InValue = DisplayManager->GetText(EG2IStringTablesTypes::Options, ValueStringID);
		PropertySelector->AddPropertyValue(InValue);
	}
	PropertySelector->SelectValueByIndex(DefaultValueIndex);
}

void UG2IUIManager::SetPropertyRow(UG2INumericalMultiValuePropertyRow* PropertySelector,
	const FString& PropertyNameStringID, const float MinValue, const float MaxValue, const float Step,
	const float DefaultValue, const int32 DecimalPlaces) const
{
	if (!ensure(PropertySelector))
	{
		UE_LOG(LogG2I, Warning, TEXT("An attempt to change nullptr %s in %s"),
			*UG2INumericalMultiValuePropertyRow::StaticClass()->GetName(), *GetName());
		return;
	}
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
		return;
	}
	
	DisplayManager->SetText<URichTextBlock>(PropertySelector->PropertyName, EG2IStringTablesTypes::Options,
		PropertyNameStringID, "PropertyName");
	PropertySelector->InitializeRow(MinValue, MaxValue, Step, DefaultValue, DecimalPlaces);
}

void UG2IUIManager::ApplyPropertiesValues(TArray<UG2IPropertyRow*> Properties) const
{
	for (const UG2IPropertyRow *PropertyRow : Properties)
	{
		if (!ensure(PropertyRow))
		{
			UE_LOG(LogG2I, Warning, TEXT("Attempting to change nullptr %s in %s"),
				*UG2IPropertyRow::StaticClass()->GetName(), *GetName());
			return;
		}
		
		ApplyPropertyValue(PropertyRow);
	}
}

void UG2IUIManager::CancelUnAppliedPropertiesValues(TArray<UG2IPropertyRow*> Properties) const
{
	for (const UG2IPropertyRow *PropertyRow : Properties)
	{
		if (!ensure(PropertyRow))
		{
			UE_LOG(LogG2I, Warning, TEXT("Attempting to change nullptr %s in %s"),
				*UG2IPropertyRow::StaticClass()->GetName(), *GetName());
			return;
		}
		
		CancelUnAppliedPropertyValue(PropertyRow);
	}
}

void UG2IUIManager::ApplyPropertyValue(const UG2IPropertyRow* PropertyRow) const
{
	if (!ensure(PropertyRow))
	{
		UE_LOG(LogG2I, Warning, TEXT("Attempting to change nullptr %s in %s"),
			*UG2IPropertyRow::StaticClass()->GetName(), *GetName());
		return;
	}

	if (PropertyRow->OnApplyPropertyValue)
	{
		PropertyRow->OnApplyPropertyValue();
	}
}

void UG2IUIManager::CancelUnAppliedPropertyValue(const UG2IPropertyRow* PropertyRow) const
{
	if (!ensure(PropertyRow))
	{
		UE_LOG(LogG2I, Warning, TEXT("Attempting to change nullptr %s in %s"),
			*UG2IPropertyRow::StaticClass()->GetName(), *GetName());
		return;
	}

	if (PropertyRow->OnCancelUnAppliedPropertyValue)
	{
		PropertyRow->OnCancelUnAppliedPropertyValue();
	}
}

void UG2IUIManager::SetActionControl(const FText& ActionName, const FText& KeyName, UListView* ControlsList) const
{
	if (!ensure(ControlsList))
	{
		UE_LOG(LogG2I, Warning, TEXT("Attempting to change nullptr %s in %s"),
			*UListView::StaticClass()->GetName(), *GetName());
		return;
	}
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
		return;
	}
	
	UG2IControlListItem *ListEntry = NewObject<UG2IControlListItem>();
	if (!ensure(ListEntry))
	{
		UE_LOG(LogG2I, Error,  TEXT("Failed to create %s in %s"),
			*UG2IControlListItem::StaticClass()->GetName(), *GetName());
		return;
	}
	
	ListEntry->ActionName = ActionName;
	ListEntry->KeyName = KeyName;
	if (ControlsList->GetEntryWidgetClass() &&
		ControlsList->GetEntryWidgetClass()->IsChildOf(UG2IControlRow::StaticClass()))
	{
		ControlsList->SetSelectedItem(ListEntry);
	}
}

void UG2IUIManager::ApplyAllOptions(const UWidgetSwitcher* OptionsSubWidgetSwitcher) const
{
	if (!ensure(OptionsSubWidgetSwitcher))
	{
		UE_LOG(LogG2I, Warning, TEXT("An attempt to change nullptr %s in %s"),
			*UWidgetSwitcher::StaticClass()->GetName(), *GetName());
		return;
	}
	for (UWidget *OptionsSubWidget : OptionsSubWidgetSwitcher->GetAllChildren())
	{
		ApplyOptions(OptionsSubWidget);
	}
}

void UG2IUIManager::CancelAllUnAppliedOptions(const UWidgetSwitcher* OptionsSubWidgetSwitcher) const
{
	if (!ensure(OptionsSubWidgetSwitcher))
	{
		UE_LOG(LogG2I, Warning, TEXT("An attempt to change nullptr %s in %s"),
			*UWidgetSwitcher::StaticClass()->GetName(), *GetName());
		return;
	}
	for (UWidget *OptionsSubWidget : OptionsSubWidgetSwitcher->GetAllChildren())
	{
		CancelUnAppliedOptions(OptionsSubWidget);
	}
}

void UG2IUIManager::ApplyOptions(UWidget* Widget)
{
	if (!Widget->Implements<UG2IUpdateOptions>())
	{
		return;
	}
	IG2IUpdateOptions::Execute_ApplyOptions(Widget);
}

void UG2IUIManager::CancelUnAppliedOptions(UWidget* Widget)
{
	if (!Widget->Implements<UG2IUpdateOptions>())
	{
		return;
	}
	IG2IUpdateOptions::Execute_CancelUnAppliedOptions(Widget);
}

void UG2IUIManager::SetupPauseWidget(const TFunction<void()>& NewContinueAction) const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIDisplayManager::StaticClass()->GetName());
		return;
	}
	if (UG2IPauseWidget *Widget = Cast<UG2IPauseWidget>(
		DisplayManager->GetWidget(EG2IWidgetNames::Pause)))
	{
		Widget->OnContinue = NewContinueAction;
	}
}

void UG2IUIManager::SetupGalleryWidget(const TFunction<void()>& NewBackAction) const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIDisplayManager::StaticClass()->GetName());
		return;
	}
	if (UG2IGalleryWidget *Widget = Cast<UG2IGalleryWidget>(
		DisplayManager->GetWidget(EG2IWidgetNames::Gallery)))
	{
		Widget->OnBack = NewBackAction;
	}
}

void UG2IUIManager::SetupControlsWidget(UWidgetSwitcher* CharacterControlsSwitcher) const
{
	if (!ensure(CharacterControlsSwitcher))
	{
		UE_LOG(LogG2I, Warning, TEXT("Attempting to change nullptr %s in %s"),
			*UWidgetSwitcher::StaticClass()->GetName(), *GetName());
		return;
	}
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
		return;
	}
	
	if (UG2ICharacterControlsWidget *Widget =
		Cast<UG2ICharacterControlsWidget>(DisplayManager->GetWidget(EG2IWidgetNames::EngineerControlsOptions)))
	{
		Widget->Character = AG2ICharacterEngineer::StaticClass();
		CharacterControlsSwitcher->AddChild(Widget);
	}
	if (UG2ICharacterControlsWidget *Widget =
		Cast<UG2ICharacterControlsWidget>(DisplayManager->GetWidget(EG2IWidgetNames::DaughterControlsOptions)))
	{
		Widget->Character = AG2ICharacterDaughter::StaticClass();
		CharacterControlsSwitcher->AddChild(Widget);
	}
}

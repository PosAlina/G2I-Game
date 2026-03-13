#include "G2IUIManager.h"
#include "G2I.h"
#include "G2IAimTypeEnum.h"
#include "G2IGameInstance.h"
#include "G2IPlayerController.h"
#include "G2IStringTablesTypes.h"
#include "G2IUIDisplayManager.h"
#include "G2IWidgetComponentParameters.h"
#include "G2IWidgetNames.h"
#include "G2IWorldHintKeyWidgetComponent.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Gameplay/G2IKeyHintWidget.h"
#include "HUD/G2IAimingWidget.h"
#include "Menu/Elements/TextRow/G2ITextMultiValuePropertyRow.h"

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
	
	UG2IGameInstance *GameInstance = Cast<UG2IGameInstance>(GetGameInstance());
	if (!ensure(GameInstance))
	{
		UE_LOG(LogG2I, Error, TEXT("Game Instance isn't %s in %s"),
			*UG2IGameInstance::StaticClass()->GetName(), *GetName());
		return;
	}
	WidgetComponentParameters = GameInstance->GetWidgetComponentParameters();

	DisplayManager = NewObject<UG2IUIDisplayManager>(this);
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
		return;
	}
	DisplayManager->Initialize();

	const UWorld *World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}
	OnUIManagerInitialized.Broadcast();
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
	OpenWidget(EG2IWidgetNames::TrainingScreen);
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
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
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
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
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

void UG2IUIManager::OpenWidget(const EG2IWidgetNames WidgetName) const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
		return;
	}
	
	DisplayManager->OpenWidget(WidgetName);
}

void UG2IUIManager::CloseWidget(const EG2IWidgetNames WidgetName) const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
		return;
	}
	
	DisplayManager->CloseWidget(WidgetName);
}

void UG2IUIManager::ShowWidget(const EG2IWidgetNames WidgetName) const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
		return;
	}
	UG2IUserWidget *Widget = DisplayManager->GetWidget(WidgetName);
	if (!ensure(Widget))
	{
		UE_LOG(LogG2I, Warning, TEXT("Widget %s doesn't exist in %s"), *GetWidgetNameString(WidgetName),
			*GetName());
		return;
	}

	Widget->SetVisibility(ESlateVisibility::Visible);
}

void UG2IUIManager::HideWidget(const EG2IWidgetNames WidgetName) const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
		return;
	}
	UG2IUserWidget *Widget = DisplayManager->GetWidget(WidgetName);
	if (!ensure(Widget))
	{
		UE_LOG(LogG2I, Warning, TEXT("Widget %s doesn't exist in %s"), *GetWidgetNameString(WidgetName),
			*GetName());
		return;
	}
	
	Widget->SetVisibility(ESlateVisibility::Hidden);
}

void UG2IUIManager::CloseAllWidgets() const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
		return;
	}

	DisplayManager->CloseAllActiveWidgets();
}

void UG2IUIManager::ChangeAimingType(const EG2IAimType NewAimType) const
{
	if (!ensure(DisplayManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
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
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("PlayerController doesn't exist in %s"), *GetName());
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
		const FName Key = PlayerController->GetKeyName(InputAction);
		Widget->KeyTextBlock->SetText(FText::FromName(Key));
	}
}

void UG2IUIManager::SetKeyWidgetSize(UG2IWorldHintKeyWidgetComponent* WidgetComponent) const
{
	if (!ensure(WidgetComponent))
	{
		UE_LOG(LogG2I, Warning, TEXT("Attempting to set key in nullptr widget component in %s"), *GetName());
		return;
	}
	if (!ensure(WidgetComponentParameters))
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
			*UG2IWidgetComponentParameters::StaticClass()->GetName(), *GetName());
		return;
	}

	WidgetComponent->SetWidgetSize(WidgetComponentParameters->KeyWidgetDefaultSize);
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
		UE_LOG(LogG2I, Error, TEXT("%s doesn't exist in %s"),
			*UG2IUIDisplayManager::StaticClass()->GetName(), *GetName());
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

void UG2IUIManager::ApplyPropertiesValues(TArray<UG2IPropertyRow*> Properties) const
{
	for (const UG2IPropertyRow *PropertyRow : Properties)
	{
		ApplyPropertyValue(PropertyRow);
	}
}

void UG2IUIManager::SavePropertiesValues(TArray<UG2IPropertyRow*> Properties) const
{
	for (const UG2IPropertyRow *PropertyRow : Properties)
	{
		SavePropertyValue(PropertyRow);
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

void UG2IUIManager::SavePropertyValue(const UG2IPropertyRow* PropertyRow) const
{
	if (!ensure(PropertyRow))
	{
		UE_LOG(LogG2I, Warning, TEXT("Attempting to change nullptr %s in %s"),
			*UG2IPropertyRow::StaticClass()->GetName(), *GetName());
		return;
	}

	if (PropertyRow->OnSavePropertyValue)
	{
		PropertyRow->OnSavePropertyValue();
	}
}

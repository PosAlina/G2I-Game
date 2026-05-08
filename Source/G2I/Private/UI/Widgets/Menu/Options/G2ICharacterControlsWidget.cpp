#include "Menu/Options/G2ICharacterControlsWidget.h"
#include "G2I.h"
#include "G2IPlayerController.h"
#include "Components/ListView.h"
#include "GameFramework/Character.h"
#include "Menu/Elements/G2IControlListItem.h"
#include "Menu/Elements/G2IControlRow.h"

void UG2ICharacterControlsWidget::InitializeAfterManagerLoading()
{
	Super::InitializeAfterManagerLoading();

	InitializeDefaults();
}

void UG2ICharacterControlsWidget::InitializeDefaults() const
{
	SetCharacterControls();
}

void UG2ICharacterControlsWidget::SetCharacterControls() const
{
	if (!Character)
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Character for controls is NULL"), *GetName());
		return;
	}
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(), *AG2IPlayerController::StaticClass()->GetName());
		return;
	}
	
	TMap<TSubclassOf<APawn>, FG2IInputKeyMapping> InputKeyMappings = PlayerController->GetInputKeyMapping();

	FG2IInputKeyMapping InputKeyMappingForCharacter;
	bool bIsFindInputMapping = false;
	for (auto& [PawnClass, InputKeyMapping] :InputKeyMappings)
	{
		if (PawnClass->IsChildOf(Character))
		{
			InputKeyMappingForCharacter = InputKeyMapping;
			bIsFindInputMapping = true;
			break;
		}
	}
	if (!ensure(bIsFindInputMapping))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s:Couldn't find %s in InputKeyMappings"), *GetName(), *Character->GetName());
		return;
	}
	TArray<FEnhancedActionKeyMapping> EnhancedActionKeyMappings = InputKeyMappingForCharacter.Mappings;
	for (FEnhancedActionKeyMapping EnhancedActionKeyMapping : EnhancedActionKeyMappings)
	{
		SetRow(EnhancedActionKeyMapping.Action->ActionDescription, EnhancedActionKeyMapping.Key.GetDisplayName());
	}
}

void UG2ICharacterControlsWidget::SetRow(const FText& ActionName, const FText& KeyName) const
{
	if (!ensure(ControlsList))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find ListView in ControlList"), *GetName());
		return;
	}
	UG2IControlListItem *Item = NewObject<UG2IControlListItem>();
	if (!ensure(Item))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: ControlListItem couldn't created"), *GetName());
		return;
	}
	Item->ActionName = ActionName;
	Item->KeyName = KeyName;
	ControlsList->AddItem(Item);
}

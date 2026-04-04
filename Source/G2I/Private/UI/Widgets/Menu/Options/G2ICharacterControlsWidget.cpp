#include "Menu/Options/G2ICharacterControlsWidget.h"

#include "G2I.h"
#include "G2ICharacterDaughter.h"
#include "G2ICharacterEngineer.h"
#include "Components/ListView.h"
#include "Menu/Elements/G2IControlListItem.h"
#include "Menu/Elements/G2IControlRow.h"

void UG2ICharacterControlsWidget::InitializeAfterManagerLoading()
{
	Super::InitializeAfterManagerLoading();

	InitializeDefaults();
}

void UG2ICharacterControlsWidget::InitializeDefaults()
{
	SetCharacterControls();
}

void UG2ICharacterControlsWidget::SetCharacterControls()
{
	// TODO: Set real controls. For-loop for target input mapping contexts. Save array of mapping contexts by characters
	// Example:
	// PlayerController->GetInputMappingContexts(Character) or from User Settings of Character
	
	SetRow(FText::FromString("CommonAction1"), FText::FromString("Key1"));
	SetRow(FText::FromString("CommonAction2"), FText::FromString("Key2"));
	SetRow(FText::FromString("CommonAction3"), FText::FromString("Key3"));
	
	if (Character && Character->IsChildOf(AG2ICharacterEngineer::StaticClass()))
	{
		SetRow(FText::FromString("EngineerAction1"), FText::FromString("Key4"));
		SetRow(FText::FromString("EngineerAction2"), FText::FromString("Key5"));
		SetRow(FText::FromString("EngineerAction3"), FText::FromString("Key6"));
	}
	if (Character && Character->IsChildOf(AG2ICharacterDaughter::StaticClass()))
	{
		SetRow(FText::FromString("DaughterAction1"), FText::FromString("Key7"));
		SetRow(FText::FromString("DaughterAction2"), FText::FromString("Key8"));
		SetRow(FText::FromString("DaughterAction3"), FText::FromString("Key9"));
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

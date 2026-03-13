#include "Menu/Elements/TextRow/G2ITextPropertyComboBox.h"
#include "G2I.h"
#include "Components/ComboBoxString.h"
#include "Menu/Elements/TextRow/G2ITextPropertySelector.h"

void UG2ITextPropertyComboBox::InitializeAfterManagerLoading()
{
	Super::InitializeAfterManagerLoading();

	BindDelegates();
}

void UG2ITextPropertyComboBox::BindDelegates()
{
	if (!ensure(PropertyValueComboBox))
	{
		UE_LOG(LogG2I, Error, TEXT("Combo box of property doesn't exist in %s"), *GetName());
		return;
	}
	PropertyValueComboBox->OnSelectionChanged.AddDynamic(this, &ThisClass::OnPropertyOptionSelected);
}

void UG2ITextPropertyComboBox::OnPropertyOptionSelected(FString SelectedItem, ESelectInfo::Type SelectInfo)
{
	if (SelectInfo != GetCurrentValueIndex())
	{
		SelectValueByIndex(SelectInfo);
	}
}

void UG2ITextPropertyComboBox::AddPropertyValue(FText& PropertyValue)
{
	Super::AddPropertyValue(PropertyValue);

	if (!ensure(PropertyValueComboBox))
	{
		UE_LOG(LogG2I, Error, TEXT("Combo box of property doesn't exist in %s"), *GetName());
		return;
	}
	PropertyValueComboBox->AddOption(PropertyValue.ToString());
}

bool UG2ITextPropertyComboBox::SelectValueByIndex(const int32 Index)
{
	if (Super::SelectValueByIndex(Index))
	{
		if (!ensure(PropertyValueComboBox))
		{
			UE_LOG(LogG2I, Error, TEXT("Combo box of property doesn't exist in %s"), *GetName());
			return false;
		}
		
		PropertyValueComboBox->SetSelectedIndex(Index);
		return true;
	}
	return false;
}
#include "Menu/Elements/TextRow/G2ITextPropertyCheckBox.h"
#include "G2I.h"
#include "Components/CheckBox.h"

void UG2ITextPropertyCheckBox::InitializeAfterManagerLoading()
{
	Super::InitializeAfterManagerLoading();

	BindDelegates();
}

void UG2ITextPropertyCheckBox::BindDelegates()
{
	if (!ensure(PropertyValuesCheckBox))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Check box of PropertyValuesCheckBox"), *GetName());
		return;
	}
	PropertyValuesCheckBox->OnCheckStateChanged.AddDynamic(this, &ThisClass::OnPropertyCheckBoxChanged);
}

void UG2ITextPropertyCheckBox::AddPropertyValue(FText& PropertyValue)
{
	if (!ensure(IsEmpty() || (GetMaxValueIndex() < 2)))
	{
		UE_LOG(LogG2I, Warning, TEXT("Attempting to set more than two values in PropertyValuesCheckBox in %s"),
			*GetName());
		return;
	}
	Super::AddPropertyValue(PropertyValue);
}

bool UG2ITextPropertyCheckBox::SelectValueByIndex(const int32 Index)
{
	if (Super::SelectValueByIndex(Index))
	{
		if (!ensure(PropertyValuesCheckBox))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Check box of PropertyValuesCheckBox"), *GetName());
			return false;
		}
		PropertyValuesCheckBox->SetIsChecked(Index == 1);
		return true;
	}
	return false;
}

void UG2ITextPropertyCheckBox::OnPropertyCheckBoxChanged(const bool bIsChecked)
{
	const int32 NewIndex = bIsChecked;
	if (NewIndex != GetCurrentValueIndex())
	{
		SelectValueByIndex(NewIndex);
	}
}

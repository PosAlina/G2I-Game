#include "Menu/Elements/TextRow/G2ITextPropertySelector.h"
#include "G2I.h"
#include "Components/Button.h"
#include "Components/RichTextBlock.h"

void UG2ITextPropertySelector::InitializeAfterManagerLoading()
{
	Super::InitializeAfterManagerLoading();

	BindDelegatesSelector();
}

void UG2ITextPropertySelector::BindDelegatesSelector()
{
	if (ensure(LastButton))
	{
		LastButton->OnClicked.AddDynamic(this, &ThisClass::OnLastButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("Button for switching to last value of property doesn't exist in %s"), *GetName());
	}
	if (ensure(NextButton))
	{
		NextButton->OnClicked.AddDynamic(this, &ThisClass::OnNextButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("Button for switching to next value of property doesn't exist in %s"), *GetName());
	}
}

void UG2ITextPropertySelector::OnLastButtonClicked()
{
	SelectLastValue();
}

void UG2ITextPropertySelector::OnNextButtonClicked()
{
	SelectNextValue();
}

bool UG2ITextPropertySelector::SelectValueByIndex(const int32 Index)
{
	if (Super::SelectValueByIndex(Index))
	{
		if (!ensure(ValueTextBox))
		{
			UE_LOG(LogG2I, Error, TEXT("Value box of property doesn't exist in %s"), *GetName());
			return false;
		}
		ValueTextBox->SetText(GetCurrentValue());
		SetButtonsIsEnabledByIndex(GetCurrentValueIndex());
		return true;
	}
	return false;
}

void UG2ITextPropertySelector::SelectValue(FText& ValueText)
{
	const int32 Index = GetIndexByValue(ValueText);
	if (Index != -1)
	{
		SelectValueByIndex(Index);
	}
}

void UG2ITextPropertySelector::SetButtonsIsEnabledByIndex(const int32 Index) const
{
	if (!ensure(LastButton))
	{
		UE_LOG(LogG2I, Error, TEXT("Button for switching to last value of property doesn't exist in %s"), *GetName());
	}
	if (!ensure(NextButton))
	{
		UE_LOG(LogG2I, Error, TEXT("Button for switching to next value of property doesn't exist in %s"), *GetName());
	}
	
	LastButton->SetIsEnabled(Index != 0);
	NextButton->SetIsEnabled(Index != PropertyValues.Num() - 1);
}

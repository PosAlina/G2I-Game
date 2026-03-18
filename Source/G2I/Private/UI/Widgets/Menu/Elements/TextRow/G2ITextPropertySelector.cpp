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
	if (ensure(PreviousButton))
	{
		PreviousButton->OnClicked.AddDynamic(this, &ThisClass::OnPreviousButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find PreviousButton"), *GetName());
	}
	if (ensure(NextButton))
	{
		NextButton->OnClicked.AddDynamic(this, &ThisClass::OnNextButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find NextButton"), *GetName());
	}
}

void UG2ITextPropertySelector::OnPreviousButtonClicked()
{
	SelectPreviousValue();
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
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Text Box in ValueTextBox"), *GetName());
			return false;
		}
		ValueTextBox->SetText(GetCurrentValue());
		SetButtonsAreEnabledByIndex(GetCurrentValueIndex());
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

void UG2ITextPropertySelector::SetButtonsAreEnabledByIndex(const int32 Index) const
{
	if (!ensure(PreviousButton))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find PreviousButton"), *GetName());
	}
	else
	{
		PreviousButton->SetIsEnabled(Index != 0);
	}
	if (!ensure(NextButton))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find NextButton"), *GetName());
	}
	else
	{
		NextButton->SetIsEnabled(Index != PropertyValues.Num() - 1);
	}
}

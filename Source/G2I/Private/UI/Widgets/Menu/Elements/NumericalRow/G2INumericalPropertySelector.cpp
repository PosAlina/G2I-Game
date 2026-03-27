#include "Menu/Elements/NumericalRow/G2INumericalPropertySelector.h"
#include "G2I.h"
#include "Components/Button.h"
#include "Components/RichTextBlock.h"

void UG2INumericalPropertySelector::InitializeAfterManagerLoading()
{
	Super::InitializeAfterManagerLoading();

	BindDelegates();
}

void UG2INumericalPropertySelector::BindDelegates()
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

void UG2INumericalPropertySelector::InitializeRow(const float InMinValue, const float InMaxValue,
	const float InStepValue, const float InDefaultValue, const float InDecimalPlaces)
{
	Super::InitializeRow(InMinValue, InMaxValue, InStepValue, InDefaultValue, InDecimalPlaces);
	
	SetButtonsIsEnabledByValue();
}

void UG2INumericalPropertySelector::OnPreviousButtonClicked()
{
	SelectPreviousValue();
}

void UG2INumericalPropertySelector::OnNextButtonClicked()
{
	SelectNextValue();
}

bool UG2INumericalPropertySelector::SelectValue(const float InValue)
{
	if (Super::SelectValue(InValue))
	{
		if (!ensure(ValueTextBox))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Text Box in ValueTextBox"), *GetName());
			return false;
		}
		ValueTextBox->SetText(FText::AsNumber(GetCurrentValue()));
		SetButtonsIsEnabledByValue();
		return true;
	}
	return false;
}

void UG2INumericalPropertySelector::SetButtonsIsEnabledByValue() const
{
	const float Value = GetCurrentValue();
	if (!ensure(PreviousButton))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find PreviousButton"), *GetName());
	}
	else
	{
		const bool bIsValueEqualMin = FMath::IsNearlyEqual(Value, GetMinValue(), GetStepValue() / 2);
		PreviousButton->SetIsEnabled(Value > GetMinValue() && !bIsValueEqualMin);

	}
	if (!ensure(NextButton))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find NextButton"), *GetName());
	}
	else
	{
		const bool bIsValueEqualMax = FMath::IsNearlyEqual(Value, GetMaxValue(), GetStepValue() / 2);
		NextButton->SetIsEnabled(Value < GetMaxValue() && !bIsValueEqualMax);
	}
}
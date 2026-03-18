#include "Menu/Elements/NumericalRow/G2INumericalPropertySlider.h"
#include "G2I.h"
#include "Components/Slider.h"

void UG2INumericalPropertySlider::InitializeAfterManagerLoading()
{
	Super::InitializeAfterManagerLoading();

	BindDelegatesSlider();
}

void UG2INumericalPropertySlider::BindDelegatesSlider()
{
	if (!ensure(PropertyValuesSlider))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Slider in PropertyValueSlider"), *GetName());
		return;
	}
	PropertyValuesSlider->OnValueChanged.AddDynamic(this, &ThisClass::OnValueChanged);
}

void UG2INumericalPropertySlider::InitializeRow(const float InMinValue, const float InMaxValue, const float InStepValue,
                                                const float InDefaultValue, const float InDecimalPlaces)
{
	Super::InitializeRow(InMinValue, InMaxValue, InStepValue, InDefaultValue, InDecimalPlaces);

	if (!ensure(PropertyValuesSlider))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Slider in PropertyValueSlider"), *GetName());
		return;
	}
	PropertyValuesSlider->SetMinValue(GetMinValue());
	PropertyValuesSlider->SetMaxValue(GetMaxValue());
	PropertyValuesSlider->SetStepSize(GetStepValue());
}

bool UG2INumericalPropertySlider::SelectValue(const float InValue)
{
	if (Super::SelectValue(InValue))
	{
		if (!ensure(PropertyValuesSlider))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Slider in PropertyValueSlider"), *GetName());
			return false;
		}
		PropertyValuesSlider->SetValue(GetCurrentValue());
		return true;
	}
	return false;
}

void UG2INumericalPropertySlider::OnValueChanged(const float InNewValue)
{
	if (FMath::IsNearlyEqual(InNewValue, GetCurrentValue()))
	{
		return;
	}
	SelectValue(InNewValue);
}


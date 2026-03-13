#include "Menu/Elements/TextRow/G2ITextPropertySlider.h"
#include "G2I.h"
#include "Components/Slider.h"

void UG2ITextPropertySlider::InitializeAfterManagerLoading()
{
	Super::InitializeAfterManagerLoading();
	
	InitializeDefaults();
	BindDelegatesSlider();
}

void UG2ITextPropertySlider::InitializeDefaults() const
{
	if (!ensure(PropertyValuesSlider))
	{
		UE_LOG(LogG2I, Error, TEXT("Slider of property doesn't exist in %s"), *GetName());
		return;
	}
    PropertyValuesSlider->SetMinValue(0);
    PropertyValuesSlider->SetMaxValue(GetMaxValueIndex());
    PropertyValuesSlider->SetStepSize(1);
}

void UG2ITextPropertySlider::BindDelegatesSlider()
{
	if (!ensure(PropertyValuesSlider))
	{
		UE_LOG(LogG2I, Error, TEXT("Slider of property doesn't exist in %s"), *GetName());
		return;
	}
	PropertyValuesSlider->OnValueChanged.AddDynamic(this, &ThisClass::OnValueChanged);
}

void UG2ITextPropertySlider::AddPropertyValue(FText& PropertyValue)
{
	Super::AddPropertyValue(PropertyValue);

	if (!ensure(PropertyValuesSlider))
	{
		UE_LOG(LogG2I, Error, TEXT("Slider of property doesn't exist in %s"), *GetName());
		return;
	}
	PropertyValuesSlider->SetMaxValue(GetMaxValueIndex());
}

bool UG2ITextPropertySlider::SelectValueByIndex(const int32 Index)
{
	if (Super::SelectValueByIndex(Index))
	{
		if (!ensure(PropertyValuesSlider))
		{
			UE_LOG(LogG2I, Error, TEXT("Slider of property doesn't exist in %s"), *GetName());
			return false;
		}
		PropertyValuesSlider->SetValue(GetCurrentValueIndex());
		return true;
	}
	return false;
}

void UG2ITextPropertySlider::OnValueChanged(float InNewValue)
{
	if (FMath::IsNearlyEqual(InNewValue, GetCurrentValueIndex()))
	{
		return;
	}
	SelectValueByIndex(static_cast<int32>(InNewValue));
}
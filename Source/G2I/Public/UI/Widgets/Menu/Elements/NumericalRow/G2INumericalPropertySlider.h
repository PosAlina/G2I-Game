#pragma once

#include "CoreMinimal.h"
#include "Menu/Elements/NumericalRow/G2INumericalPropertySelector.h"
#include "G2INumericalPropertySlider.generated.h"

class USlider;

UCLASS()
class G2I_API UG2INumericalPropertySlider : public UG2INumericalPropertySelector
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> PropertyValuesSlider;

public:
	
	virtual void InitializeRow(const float InMinValue, const float InMaxValue, const float InStepValue,
		const float InDefaultValue, const float InDecimalPlaces) override;
	virtual bool SelectValue(const float InValue) override;
	
protected:

	virtual void InitializeAfterManagerLoading() override;
	
	UFUNCTION()
	void OnValueChanged(float InNewValue);

private:

	void BindDelegatesSlider();
	
};
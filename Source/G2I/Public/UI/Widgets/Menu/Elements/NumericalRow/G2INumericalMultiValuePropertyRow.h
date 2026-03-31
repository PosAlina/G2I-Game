#pragma once

#include "CoreMinimal.h"
#include "Menu/Elements/G2IPropertyRow.h"
#include "G2INumericalMultiValuePropertyRow.generated.h"

UCLASS()
class G2I_API UG2INumericalMultiValuePropertyRow : public UG2IPropertyRow
{
	GENERATED_BODY()
	
public:
	
	bool bIsInitialized = false;

private:

	float MinValue = 0.f;
	float MaxValue = 0.f;
	float StepValue = 1.f;
	float CurrentValue = 0.f;
	int32 DecimalPlaces = 0.f;
	int32 StepIndex = 0;

public:

	virtual void InitializeRow(const float InMinValue, const float InMaxValue, const float InStepValue,
		const float InDefaultValue, const float InDecimalPlaces);
	
	virtual bool SelectValue(const float InValue);
	virtual bool SelectPreviousValue();
	virtual bool SelectNextValue();

	int32 GetStepsCountUpToValue(const float InValue) const;
	int32 GetStepIndex() const;
	float GetCurrentValue() const;
	float GetMinValue() const;
	float GetMaxValue() const;
	float GetStepValue() const;
	int32 GetDecimalPlaces() const;
};

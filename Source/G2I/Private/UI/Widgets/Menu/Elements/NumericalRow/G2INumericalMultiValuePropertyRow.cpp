#include "Menu/Elements/NumericalRow/G2INumericalMultiValuePropertyRow.h"
#include "G2I.h"

void UG2INumericalMultiValuePropertyRow::InitializeRow(const float InMinValue, const float InMaxValue,
                                                       const float InStepValue, const float InDefaultValue,
                                                       const float InDecimalPlaces)
{
	MinValue = InMinValue;
	MaxValue = InMaxValue;
	
	const bool bPosDirection = MinValue < MaxValue && StepValue > 0;
	const bool bNegDirection = MinValue > MaxValue && StepValue < 0;
	const bool bZeroRow = MinValue == MaxValue && StepValue == 0;
	if (!ensure(bPosDirection || bNegDirection || bZeroRow))
	{
		UE_LOG(LogG2I, Warning, TEXT("An attempt to get count of steps up with impossible "
			"parameters in %s: Max value %f, Min value %f, Step %f"), *GetName(), MinValue, MaxValue, StepValue);
		return;
	}
	StepValue = InStepValue;
	
	if (InDecimalPlaces >= 0)
	{
		DecimalPlaces = InDecimalPlaces;
	}

	bIsInitialized = true;
	if (!ensure(GetStepsCountUpToValue(InMaxValue) != -1))
	{
		UE_LOG(LogG2I, Warning, TEXT("Impossible step's count 0 for parameters in %s:"
			"Max value %f, Min value %f, Step %f"), *GetName(), MinValue, MaxValue, StepValue);
		bIsInitialized = false;
		return;
	}

	if (!ensure(SelectValue(InDefaultValue)))
	{
		UE_LOG(LogG2I, Warning, TEXT("Impossible defaults value %f for parameters in %s:"
			"Max value %f, Min value %f, Step %f"), InDefaultValue, *GetName(), MinValue, MaxValue, StepValue);
		bIsInitialized = false;
		return;
	}
}

int32 UG2INumericalMultiValuePropertyRow::GetStepsCountUpToValue(const float InValue) const
{
	const bool bValueBetweenMinAndMax = StepValue > 0 &&
		FMath::IsWithinInclusive(InValue, MinValue, MaxValue);
	const bool bValueBetweenMaxAndMin = StepValue < 0 &&
		FMath::IsWithinInclusive(InValue, MaxValue, MinValue);
	if (!ensure(bValueBetweenMaxAndMin || bValueBetweenMinAndMax))
	{
		UE_LOG(LogG2I, Warning, TEXT("An attempt to get count of steps up to value %f with impossible "
			"parameters in %s: Max value %f, Min value %f, Step %f"),
			InValue, *GetName(), MinValue, MaxValue, StepValue);
		return -1;
	}
	
	const float Difference = InValue - MinValue;
	const float FloatStepsCount = Difference / StepValue;
	const float StepsCount = FMath::RoundToFloat(FloatStepsCount);
	if (!FMath::IsNearlyEqual(FloatStepsCount, StepsCount, StepValue))
	{
		return -1;
	}
	return static_cast<int32>(StepsCount);
}

bool UG2INumericalMultiValuePropertyRow::SelectValue(const float InValue)
{
	if (!bIsInitialized)
	{
		return false;
	}
	
	if (DecimalPlaces == 0)
	{
		CurrentValue = FMath::RoundToInt(InValue);
	}
	else
	{
		const float Multiplier = FMath::Pow(10.f, DecimalPlaces);
		CurrentValue = FMath::RoundToFloat(InValue * Multiplier) / Multiplier;
	}
	StepIndex = GetStepsCountUpToValue(CurrentValue);

	return true;
}

bool UG2INumericalMultiValuePropertyRow::SelectPreviousValue()
{
	return SelectValue(CurrentValue - StepValue);
}

bool UG2INumericalMultiValuePropertyRow::SelectNextValue()
{
	return SelectValue(CurrentValue + StepValue);
}

float UG2INumericalMultiValuePropertyRow::GetCurrentValue() const
{
	return CurrentValue;
}

float UG2INumericalMultiValuePropertyRow::GetMinValue() const
{
	return MinValue;
}

float UG2INumericalMultiValuePropertyRow::GetMaxValue() const
{
	return MaxValue;
}

float UG2INumericalMultiValuePropertyRow::GetStepValue() const
{
	return StepValue;
}

int32 UG2INumericalMultiValuePropertyRow::GetDecimalPlaces() const
{
	return DecimalPlaces;
}

int32 UG2INumericalMultiValuePropertyRow::GetStepIndex() const
{
	return StepIndex;
}

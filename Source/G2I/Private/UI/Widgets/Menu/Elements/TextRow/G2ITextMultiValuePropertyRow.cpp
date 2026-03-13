#include "Menu/Elements/TextRow/G2ITextMultiValuePropertyRow.h"

bool UG2ITextMultiValuePropertyRow::SelectValueByIndex(const int32 Index)
{
	if (MaxValueIndex == -1 || MaxValueIndex < Index || Index < 0)
	{
		return false;
	}

	CurrentValueIndex = Index;
	OnSelectValue.Broadcast(CurrentValueIndex);
	return true;
}

bool UG2ITextMultiValuePropertyRow::SelectLastValue()
{
	if (MaxValueIndex == -1 || CurrentValueIndex == 0)
	{
		return false;
	}
	
	const int32 NewIndex = CurrentValueIndex - 1;
	return SelectValueByIndex(NewIndex);
}

bool UG2ITextMultiValuePropertyRow::SelectNextValue()
{
	if (MaxValueIndex == -1 || CurrentValueIndex == MaxValueIndex)
	{
		return false;
	}
	
	const int32 NewIndex = CurrentValueIndex + 1;
	return SelectValueByIndex(NewIndex);
}

void UG2ITextMultiValuePropertyRow::SetDefaultValue(const int32 Value)
{
	DefaultValueIndex = Value;
	SelectValueByIndex(Value);
}

void UG2ITextMultiValuePropertyRow::SetMaxValueIndex(const int32 Value)
{
	MaxValueIndex = Value;
}

int32 UG2ITextMultiValuePropertyRow::GetCurrentValueIndex() const
{
	return CurrentValueIndex;
}

int32 UG2ITextMultiValuePropertyRow::GetDefaultValueIndex() const
{
	return DefaultValueIndex;
}

int32 UG2ITextMultiValuePropertyRow::GetMaxValueIndex() const
{
	return MaxValueIndex;
}

bool UG2ITextMultiValuePropertyRow::IsEmpty() const
{
	return MaxValueIndex == -1;
}

void UG2ITextMultiValuePropertyRow::UpdateMaxValueIndex()
{
	SetMaxValueIndex(PropertyValues.Num() - 1);
}

void UG2ITextMultiValuePropertyRow::AddPropertyValue(FText& PropertyValue)
{
	PropertyValues.Add(PropertyValue);
	UpdateMaxValueIndex();
}

int32 UG2ITextMultiValuePropertyRow::GetIndexByValue(FText& PropertyValue)
{
	for (int32 Index = 0; Index < PropertyValues.Num(); ++Index)
	{
		if (PropertyValues[Index].EqualTo(PropertyValue))
		{
			return Index;
		}
	}
	return -1;
}

FText UG2ITextMultiValuePropertyRow::GetCurrentValue() const
{
	const int32 CurrentIndex =GetCurrentValueIndex();
	if (IsEmpty() || !PropertyValues.IsValidIndex(CurrentIndex))
	{
		return FText::GetEmpty();
	}
	return PropertyValues[CurrentIndex];
}

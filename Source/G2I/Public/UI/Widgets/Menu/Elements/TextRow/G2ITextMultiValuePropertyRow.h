#pragma once

#include "CoreMinimal.h"
#include "../G2IPropertyRow.h"
#include "G2ITextMultiValuePropertyRow.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSelectValue, const int32, ValueIndex);

UCLASS()
class G2I_API UG2ITextMultiValuePropertyRow : public UG2IPropertyRow
{
	GENERATED_BODY()

public:

	FSelectValue OnSelectValue;

	UPROPERTY()
	TArray<FText> PropertyValues;

private:
	
	int32 CurrentValueIndex = 0;
	int32 DefaultValueIndex = 0;
	int32 MaxValueIndex = -1;

public:
	
	virtual bool SelectValueByIndex(const int32 Index);
	virtual bool SelectLastValue();
	virtual bool SelectNextValue();

	virtual void SetDefaultValue(const int32 Value);
	virtual void UpdateMaxValueIndex();

	virtual void AddPropertyValue(FText& PropertyValue);
	virtual int32 GetIndexByValue(FText& PropertyValue);

	int32 GetCurrentValueIndex() const;
	int32 GetDefaultValueIndex() const;
	int32 GetMaxValueIndex() const;
	FText GetCurrentValue() const;

	bool IsEmpty() const;

protected:
	
	void SetMaxValueIndex(const int32 Value);

};

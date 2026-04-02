#pragma once

#include "CoreMinimal.h"
#include "G2ITextMultiValuePropertyRow.h"
#include "G2ITextPropertyCheckBox.generated.h"

class UCheckBox;

UCLASS()
class G2I_API UG2ITextPropertyCheckBox : public UG2ITextMultiValuePropertyRow
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCheckBox> PropertyValuesCheckBox;

public:

	// First value is false. Second value is true
	virtual void AddPropertyValue(FText& PropertyValue) override;
	virtual bool SelectValueByIndex(const int32 Index) override;

protected:
	
	virtual void InitializeAfterManagerLoading() override;
	
	UFUNCTION()
	void OnPropertyCheckBoxChanged(const bool bIsChecked);

private:

	void BindDelegates();
	
};

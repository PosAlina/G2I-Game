#pragma once

#include "CoreMinimal.h"
#include "G2INumericalMultiValuePropertyRow.h"
#include "G2INumericalPropertySelector.generated.h"

class UButton;

UCLASS()
class G2I_API UG2INumericalPropertySelector : public UG2INumericalMultiValuePropertyRow
{
	GENERATED_BODY()
	
public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> PreviousButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> NextButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> ValueTextBox;

public:

	virtual void InitializeRow(const float InMinValue, const float InMaxValue, const float InStepValue, const float InDefaultValue, const float InDecimalPlaces) override;
	virtual bool SelectValue(const float InValue) override;

protected:
	
	virtual void InitializeAfterManagerLoading() override;
	
	UFUNCTION()
	void OnPreviousButtonClicked();

	UFUNCTION()
	void OnNextButtonClicked();

private:
	
	void SetButtonsIsEnabledByValue() const;
	void BindDelegates();
	
};

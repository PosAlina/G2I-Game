#pragma once

#include "CoreMinimal.h"
#include "G2ITextMultiValuePropertyRow.h"
#include "G2ITextPropertyComboBox.generated.h"

class UComboBoxString;

UCLASS()
class G2I_API UG2ITextPropertyComboBox : public UG2ITextMultiValuePropertyRow
{
	GENERATED_BODY()

public:

	UPROPERTY(Transient, meta = (BindWidget))
	TObjectPtr<UComboBoxString> PropertyValueComboBox;

public:

	virtual void AddPropertyValue(FText& PropertyValue) override;
	virtual bool SelectValueByIndex(const int32 Index) override;

protected:
	
	virtual void InitializeAfterManagerLoading() override;
	
	UFUNCTION()
	void OnPropertyOptionSelected(FString SelectedItem, ESelectInfo::Type SelectInfo);

private:

	void BindDelegates();
	
};
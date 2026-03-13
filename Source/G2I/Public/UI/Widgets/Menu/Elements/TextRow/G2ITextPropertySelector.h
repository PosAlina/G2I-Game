#pragma once

#include "CoreMinimal.h"
#include "G2ITextMultiValuePropertyRow.h"
#include "G2ITextPropertySelector.generated.h"

class UButton;
class URichTextBlock;

/**
 * 
 */
UCLASS()
class G2I_API UG2ITextPropertySelector : public UG2ITextMultiValuePropertyRow
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> LastButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> NextButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> ValueTextBox;

public:

	virtual bool SelectValueByIndex(const int32 Index) override;
	
protected:
	
	virtual void InitializeAfterManagerLoading() override;

	UFUNCTION()
	void OnLastButtonClicked();

	UFUNCTION()
	void OnNextButtonClicked();
	void SelectValue(FText& ValueText);

private:
	
	void SetButtonsIsEnabledByIndex(const int32 Index) const;
	void BindDelegatesSelector();
};
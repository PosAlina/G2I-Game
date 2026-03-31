#pragma once

#include "CoreMinimal.h"
#include "G2IUserWidget.h"
#include "G2IConfirmationWidget.generated.h"

class UButton;
class URichTextBlock;

/**
 * Widget for different confirmations. Button press confirmation
 */
UCLASS()
class G2I_API UG2IConfirmationWidget : public UG2IUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> QuestionTextBlock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ConfirmButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> ConfirmTextBlock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CancelButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> CancelTextBlock;

public:
	
	TFunction<void()> OnConfirm;
	TFunction<void()> OnCancel;

protected:
	
	virtual void InitializeAfterManagerLoading() override;

	UFUNCTION()
	void OnConfirmButtonClicked();

	UFUNCTION()
	void OnCancelButtonClicked();

private:
	
	void BindDelegates();
	
};

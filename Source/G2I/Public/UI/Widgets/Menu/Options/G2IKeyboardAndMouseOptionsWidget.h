#pragma once

#include "CoreMinimal.h"
#include "G2IOptionsBaseSubWidget.h"
#include "G2IKeyboardAndMouseOptionsWidget.generated.h"

class UG2INumericalMultiValuePropertyRow;
class UG2ITextMultiValuePropertyRow;

UCLASS()
class G2I_API UG2IKeyboardAndMouseOptionsWidget : public UG2IOptionsBaseSubWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UG2INumericalMultiValuePropertyRow> MouseSensitivity;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UG2ITextMultiValuePropertyRow> VerticalMouseInversion;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UG2ITextMultiValuePropertyRow> HorizontalMouseInversion;

public:

	virtual void ApplyOptions_Implementation() override;
	virtual void CancelUnAppliedOptions_Implementation() override;

protected:
	
	virtual void InitializeAfterManagerLoading() override;

private:
	
	void InitializeDefaults() const;
	void InitializeMouseSensitivity() const;
	void InitializeVerticalMouseInversion() const;
	void InitializeHorizontalMouseInversion() const;
	
};

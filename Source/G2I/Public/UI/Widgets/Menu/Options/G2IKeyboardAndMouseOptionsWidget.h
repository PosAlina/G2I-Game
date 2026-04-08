#pragma once

#include "CoreMinimal.h"
#include "G2IUserWidget.h"
#include "G2IKeyboardAndMouseOptionsWidget.generated.h"

class UG2INumericalMultiValuePropertyRow;
class UG2ITextMultiValuePropertyRow;

UCLASS()
class G2I_API UG2IKeyboardAndMouseOptionsWidget : public UG2IUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UG2INumericalMultiValuePropertyRow> MouseSensitivity;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UG2ITextMultiValuePropertyRow> MouseInversion;

public:

	void ApplyPropertiesValues() const;
	void SavePropertiesValues() const;

protected:
	
	virtual void InitializeAfterManagerLoading() override;

private:
	
	void InitializeDefaults() const;
	
};

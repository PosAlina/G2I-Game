#pragma once

#include "CoreMinimal.h"
#include "G2IUserWidget.h"
#include "G2IScreenOptionsWidget.generated.h"

class UG2INumericalMultiValuePropertyRow;
class UG2ITextMultiValuePropertyRow;

UCLASS()
class G2I_API UG2IScreenOptionsWidget : public UG2IUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UG2ITextMultiValuePropertyRow> ScreenMode;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UG2ITextMultiValuePropertyRow> ScreenResolution;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UG2ITextMultiValuePropertyRow> FrameRate;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UG2INumericalMultiValuePropertyRow> Brightness;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UG2ITextMultiValuePropertyRow> VSync;

public:
	
	virtual void InitializeAfterManagerLoading() override;
	void InitializeDefaults() const;

	void ApplyPropertiesValues() const;

	void SavePropertiesValues() const;
	
};

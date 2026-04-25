#pragma once

#include "CoreMinimal.h"
#include "G2IOptionsBaseSubWidget.h"
#include "G2IScreenOptionsWidget.generated.h"

class UG2INumericalMultiValuePropertyRow;
class UG2ITextMultiValuePropertyRow;

UCLASS()
class G2I_API UG2IScreenOptionsWidget : public UG2IOptionsBaseSubWidget
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

	virtual void ApplyOptions_Implementation() override;
	virtual void CancelUnAppliedOptions_Implementation() override;
	
protected:
	
	virtual void InitializeAfterManagerLoading() override;
	
private:
	
	void InitializeDefaults() const;
	
};

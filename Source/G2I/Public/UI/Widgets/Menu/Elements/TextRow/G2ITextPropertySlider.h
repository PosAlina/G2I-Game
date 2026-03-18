#pragma once

#include "CoreMinimal.h"
#include "G2ITextPropertySelector.h"
#include "G2ITextPropertySlider.generated.h"

class USlider;

UCLASS()
class G2I_API UG2ITextPropertySlider : public UG2ITextPropertySelector
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> PropertyValuesSlider;

public:

	virtual void AddPropertyValue(FText& PropertyValue) override;
	virtual bool SelectValueByIndex(const int32 Index) override;
	
protected:
	
	virtual void InitializeAfterManagerLoading() override;

	UFUNCTION()
	void OnValueChanged(float InNewValue);

private:

	void InitializeDefaults() const;
	void BindDelegatesSlider();
	
};
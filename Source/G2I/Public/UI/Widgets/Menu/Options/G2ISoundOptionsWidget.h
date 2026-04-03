#pragma once

#include "CoreMinimal.h"
#include "G2IUserWidget.h"
#include "G2ISoundOptionsWidget.generated.h"

class UG2INumericalMultiValuePropertyRow;
class UG2ITextMultiValuePropertyRow;

UCLASS()
class G2I_API UG2ISoundOptionsWidget : public UG2IUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UG2INumericalMultiValuePropertyRow> CommonVolume;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UG2INumericalMultiValuePropertyRow> MusicVolume;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UG2INumericalMultiValuePropertyRow> EffectsVolume;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UG2INumericalMultiValuePropertyRow> DialoguesVolume;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UG2ITextMultiValuePropertyRow> Subtitles;

protected:

	int32 DefaultVolumeIndex = 10;

public:
	
	void ApplyPropertiesValues() const;
	void SavePropertiesValues() const;
	
protected:
	
	virtual void InitializeAfterManagerLoading() override;

private:
	
	void InitializeDefaults() const;
	
};

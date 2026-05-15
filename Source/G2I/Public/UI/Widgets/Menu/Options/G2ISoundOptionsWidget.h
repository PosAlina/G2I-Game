#pragma once

#include "CoreMinimal.h"
#include "G2IOptionsBaseSubWidget.h"
#include "G2ISoundOptionsWidget.generated.h"

class UG2INumericalMultiValuePropertyRow;
class UG2ITextMultiValuePropertyRow;
class UG2IGameSoundManager;

UCLASS()
class G2I_API UG2ISoundOptionsWidget : public UG2IOptionsBaseSubWidget
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

protected:
	UPROPERTY()
	TObjectPtr<UG2IGameSoundManager> SoundManager;

public:

	virtual void ApplyOptions_Implementation() override;
	virtual void CancelUnAppliedOptions_Implementation() override;
	
protected:
	
	virtual void InitializeAfterManagerLoading() override;

private:
	
	void InitializeDefaults() const;
	void InitializeSoundManager();
	void InitializeCommonVolume() const;
	void InitializeMusicVolume() const;
	void InitializeEffectsVolume() const;
	void InitializeDialoguesVolume() const;
	void InitializeVolumesData();

	float MinVolume = 0.f;
	float MaxVolume = 10.f;
	float StepVolume = 1.f;
};

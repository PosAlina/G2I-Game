#pragma once

#include "CoreMinimal.h"
#include "G2IUserWidget.h"
#include "G2IControlsOptionsWidget.generated.h"

class UButton;
class UWidgetSwitcher;

/**
 * Widget which switch controls of different characters
 */
UCLASS()
class G2I_API UG2IControlsOptionsWidget : public UG2IUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(Transient, meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> CharactersSubWidgetSwitcher;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> EngineerControlButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> DaughterControlButton;

protected:

	virtual void InitializeAfterManagerLoading() override;
	
	UFUNCTION()
	void OnEngineerControlsButtonClicked();

	UFUNCTION()
	void OnDaughterControlsButtonClicked();

private:
	
	void InitializeDefaults();
	void BindDelegates();
	
};

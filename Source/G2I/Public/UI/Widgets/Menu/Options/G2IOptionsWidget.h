#pragma once

#include "CoreMinimal.h"
#include "G2IUserWidget.h"
#include "G2IOptionsWidget.generated.h"

class UButton;
class UWidgetSwitcher;

UCLASS()
class G2I_API UG2IOptionsWidget : public UG2IUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(Transient, meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> OptionsSubWidgetSwitcher;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CommonOptionsButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ControlsOptionsButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ScreenOptionsButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> GraphicsOptionsButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SoundOptionsButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ApplyButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CancelButton;

public:

	TFunction<void()> OnBack;
	
	virtual void CloseWidget() override;

protected:

	virtual void InitializeAfterManagerLoading() override;

	UFUNCTION()
	void OnBackButtonClicked();
	
	UFUNCTION()
	void OnApplyButtonClicked();
	
	UFUNCTION()
	void OnCancelButtonClicked();

	UFUNCTION()
	void OnCommonOptionsButtonClicked();
	
	UFUNCTION()
	void OnControlsOptionsButtonClicked();

	UFUNCTION()
	void OnScreenOptionsButtonClicked();

	UFUNCTION()
	void OnGraphicsOptionsButtonClicked();

	UFUNCTION()
	void OnSoundOptionsButtonClicked();

private:

	void InitializeDefaults() const;
	void BindDelegates();
	
};

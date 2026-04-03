#pragma once

#include "CoreMinimal.h"
#include "G2IUserWidget.h"
#include "G2IPauseWidget.generated.h"

class UButton;

UCLASS()
class G2I_API UG2IPauseWidget : public UG2IUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ContinueButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> OptionsButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MainMenuButton;

protected:

	virtual void InitializeAfterManagerLoading() override;

	UFUNCTION()
	void OnContinueButtonClicked();

	UFUNCTION()
	void OnOptionsButtonClicked();

	UFUNCTION()
	void OnMainMenuButtonClicked();

private:
	
	void BindDelegates();
	
	TFunction<void()> GetShowCurrentWidgetFunction() const;
	
};

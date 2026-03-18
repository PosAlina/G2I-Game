#pragma once

#include "CoreMinimal.h"
#include "G2IUserWidget.h"
#include "G2ITrainingScreen.generated.h"

class UWidgetSwitcher;
class UVerticalBox;

UCLASS()
class G2I_API UG2ITrainingScreen : public UG2IUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> CharactersDescriptionsSwitcher;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> EngineerDescription;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> DaughterDescription;

protected:

	virtual void InitializeAfterManagerLoading() override;

	UFUNCTION()
	void OnSwitchCharactersDescription(APawn *Pawn);

private:

	void BindDelegates();
	
};

#pragma once

#include "CoreMinimal.h"
#include "G2IUserWidget.h"
#include "G2ICharacterControlsWidget.generated.h"

class UListView;

/**
 * Controls for one character
 */
UCLASS()
class G2I_API UG2ICharacterControlsWidget : public UG2IUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UListView> ControlsList;

	UPROPERTY()
	TSubclassOf<ACharacter> Character;

protected:
	
	virtual void InitializeAfterManagerLoading() override;

private:

	void InitializeDefaults() const;
	void SetCharacterControls() const;

	void SetRow(const FText& ActionName, const FText& KeyName) const;
	
};

#pragma once

#include "CoreMinimal.h"
#include "G2IUserWidget.h"
#include "G2ICreatorsWidget.generated.h"

class UButton;

UCLASS()
class G2I_API UG2ICreatorsWidget : public UG2IUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;

public:

	TFunction<void()> OnBack;

protected:
	
	virtual void InitializeAfterManagerLoading() override;

	UFUNCTION()
	void OnBackButtonClicked();

private:
	
	void BindDelegates();
	
};

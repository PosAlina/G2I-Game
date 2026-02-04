#pragma once

#include "CoreMinimal.h"
#include "G2IUserWidget.h"
#include "G2IPauseWidget.generated.h"

UCLASS()
class G2I_API UG2IPauseWidget : public UG2IUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ContinueButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitGameButton;
	
};

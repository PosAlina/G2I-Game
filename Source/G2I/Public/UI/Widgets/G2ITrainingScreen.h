#pragma once

#include "CoreMinimal.h"
#include "G2IUserWidget.h"
#include "G2ITrainingScreen.generated.h"

UCLASS()
class G2I_API UG2ITrainingScreen : public UG2IUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CommonDescriptionTextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> CharacterDescriptionTextBlock;
};

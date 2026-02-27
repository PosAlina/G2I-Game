#pragma once

#include "CoreMinimal.h"
#include "G2IUserWidget.h"
#include "G2IKeyHintWidget.generated.h"

UCLASS()
class G2I_API UG2IKeyHintWidget : public UG2IUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> KeyTextBlock;
	
};

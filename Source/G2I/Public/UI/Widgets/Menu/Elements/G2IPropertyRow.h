#pragma once

#include "CoreMinimal.h"
#include "G2IUserWidget.h"
#include "G2IPropertyRow.generated.h"

class URichTextBlock;

UCLASS()
class G2I_API UG2IPropertyRow : public UG2IUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> PropertyName;

public:
	
	TFunction<void()> OnApplyPropertyValue;
	TFunction<void()> OnSavePropertyValue;
	
};

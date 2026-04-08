#pragma once

#include "CoreMinimal.h"
#include "G2IPropertyRow.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "G2IControlRow.generated.h"

/**
 * Row with action and key descriptions in Control Option
 */
UCLASS()
class G2I_API UG2IControlRow : public UG2IPropertyRow, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> KeyName;

public:

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
};

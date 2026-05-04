#pragma once

#include "CoreMinimal.h"
#include "G2IUserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "G2ITaskRow.generated.h"

class URichTextBlock;

/**
 * Row with one task in tasks screen
 */
UCLASS()
class G2I_API UG2ITaskRow : public UG2IUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> Description;
	
protected:
	
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
};

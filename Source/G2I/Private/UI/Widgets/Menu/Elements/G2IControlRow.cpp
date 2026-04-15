#include "Menu/Elements/G2IControlRow.h"
#include "G2I.h"
#include "Components/RichTextBlock.h"
#include "Menu/Elements/G2IControlListItem.h"

void UG2IControlRow::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	if (const UG2IControlListItem *ListEntry = Cast<UG2IControlListItem>(ListItemObject))
	{
		if (!ensure(PropertyName))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find TextBlock in PropertyName"), *GetName());
			return;
		}
		if (!ensure(KeyName))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find TextBlock in KeyName"), *GetName());
			return;
		}
		
		PropertyName->SetText(ListEntry->ActionName);
		KeyName->SetText(ListEntry->KeyName);
	}
}

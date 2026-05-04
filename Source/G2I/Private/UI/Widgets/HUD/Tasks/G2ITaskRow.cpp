#include "HUD/Tasks/G2ITaskRow.h"
#include "G2I.h"
#include "Components/RichTextBlock.h"
#include "HUD/Tasks/G2ITasksListItem.h"

void UG2ITaskRow::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	if (const UG2ITasksListItem *ListEntry = Cast<UG2ITasksListItem>(ListItemObject))
	{
		if (!ensure(Description))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find TextBlock in Description"), *GetName());
			return;
		}
		Description->SetText(ListEntry->Description);
	}
}

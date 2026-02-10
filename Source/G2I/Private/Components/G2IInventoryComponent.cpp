
#include "Components/G2IInventoryComponent.h"

UG2IInventoryComponent::UG2IInventoryComponent()
{

}

bool UG2IInventoryComponent::TryPickupItem(AG2IPickupItem* Item)
{
	if (!Item)
	{
		return false;
	}

	if (Item->PickupTag != PickupItemTag)
	{
		return false;
	}

	CollectedItemIDs.Add(Item->ItemID);

	DebugPrintCollectedItems();

	Item->Destroy();

	return true;
}


void UG2IInventoryComponent::DebugPrintCollectedItems() const
{
    if (GEngine)
    {
        for (const FName& ItemId : CollectedItemIDs)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, ItemId.ToString());
        }
    }
}

bool UG2IInventoryComponent::HasItemID(FName ItemID) const
{
	return CollectedItemIDs.Contains(ItemID);
}

bool UG2IInventoryComponent::RemoveItemID(FName ItemID)
{
	return CollectedItemIDs.RemoveSingle(ItemID) > 0;
}

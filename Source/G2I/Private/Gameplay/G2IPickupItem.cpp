
#include "Gameplay/G2IPickupItem.h"
#include "Characters/G2ICharacterEngineer.h"
#include "Characters/G2ICharacterDaughter.h"
#include "Components/G2IInventoryComponent.h"
#include "G2I.h"


AG2IPickupItem::AG2IPickupItem()
{

}

void AG2IPickupItem::PickUp(AActor* Interactor)
{
	if (Interactor)
	{
		if (UG2IInventoryComponent* Inventory = Interactor->FindComponentByClass<UG2IInventoryComponent>())
		{
			Inventory->TryPickupItem(this);
		}
	}
	else
	{
		UE_LOG(LogG2I, Warning, TEXT("No interactor in %s"), *GetActorNameOrLabel());
	}
}


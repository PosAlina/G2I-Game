

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Gameplay/G2IPickupItem.h"
#include "G2IInventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class G2I_API UG2IInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UG2IInventoryComponent();

	UPROPERTY(EditAnywhere, BluePrintReadOnly, Category = "Inventory")
	FName PickupItemTag;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FName> CollectedItemIDs;

	UFUNCTION(Category = "Inventory")
	bool TryPickupItem(AG2IPickupItem* Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory Debug")
	void DebugPrintCollectedItems() const;
	
};

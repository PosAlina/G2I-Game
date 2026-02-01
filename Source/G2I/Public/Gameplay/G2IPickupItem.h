

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "G2IPickupItem.generated.h"

UCLASS()
class G2I_API AG2IPickupItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AG2IPickupItem();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	FName PickupTag;

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void PickUp(AActor* Interactor);
};

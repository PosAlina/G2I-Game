#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/G2IActivationWithOrderComponent.h"
#include "G2IActivationOrderManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActivationWithOrderEndedDelegate, AActor*, ManagerActor, bool, bSuccess);

// Manager for the actors with ActivationWithOrder component.
// One for the puzzle
UCLASS()
class G2I_API AG2IActivationOrderManager : public AActor
{
	GENERATED_BODY()

public:
	// Array of actors that this manager is responsible to.
	// Order in this array sets the order of activation
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activation with order",
		meta = (ToolTip = "Add here actors that will be activated, in needed order. Actors must include ActivationWithOder component."))
	TArray<TObjectPtr<AActor>> CorrectOrderOfActors;

	UPROPERTY(BlueprintAssignable)
	FOnActivationWithOrderEndedDelegate OnActivationWithOrderEndedDelegate;

protected:
	// Array that holds already activated actors
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Activation with order",
		meta = (ToolTip = "Actors are added here when activated, then this array is compared to the CorrectOrderOfActors."))
	TArray<TObjectPtr<AActor>> ActivatedActorsArray;

	UPROPERTY(BlueprintReadWrite, Category = "Activation with order")
	int32 CurrentIndex = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Activation with order")
	int32 NumberOfActors;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> RootSceneComponent;
	
public:
	AG2IActivationOrderManager();

	UFUNCTION(BlueprintCallable, Category = "Activation with order")
	virtual void OrderCompleted();

	UFUNCTION(BlueprintCallable, Category = "Activation with order")
	virtual void OrderFailed();

	UFUNCTION(BlueprintCallable, Category = "Activation with order")
	virtual void OrderCancelled();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void BindToAllDelegates();

	// Handles Activation/Reactivation logic.
	// By default - ignores reactivation
	UFUNCTION()
	virtual void OnActorActivated(AActor* ActivatedActor, bool bReactivation, UG2IActivationWithOrderComponent* ActivationComponent);

	UFUNCTION(BlueprintCallable, Category = "Activation with order")
	void CheckIfOrderCompleted();
};

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/G2IActivationOrderManager.h"
#include "G2IActivationOrderManagerSounds.generated.h"

/**
 * Activation Order Manager, plays sounds on order completion
 * & allows to cancel last activated actor
 */
UCLASS()
class G2I_API AG2IActivationOrderManagerSounds : public AG2IActivationOrderManager
{
	GENERATED_BODY()

public:
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activation with order")
	TObjectPtr<USoundBase> CorrectOrderSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activation with order")
	TObjectPtr<USoundBase> FailedOrderSound;

public:
	virtual void OrderCompleted() override;

	virtual void OrderFailed() override;

protected:
	virtual void OnActorActivated(AActor* ActivatedActor, bool bReactivation, UG2IActivationWithOrderComponent* ActivationComponent) override;
};

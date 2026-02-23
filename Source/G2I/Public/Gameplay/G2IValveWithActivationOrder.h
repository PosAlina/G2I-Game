#pragma once

#include "CoreMinimal.h"
#include "Gameplay/G2IValve.h"
#include "Components/G2IActivationOrderSMeshComponent.h"
#include "Interfaces/G2IActivationInterface.h"
#include "G2IValveWithActivationOrder.generated.h"

/**
 * Valve with ActivationWithOrder component.
 * Activates/Deactivates only after ActivationWithOrder component tells to do so
 * and these are handled by Activation interface
 */
UCLASS()
class G2I_API AG2IValveWithActivationOrder : public AG2IValve, public IG2IActivationInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Activation with order")
	TObjectPtr<UG2IActivationOrderSMeshComponent> ActivationOrderSMeshComponent;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Activation with order")
	bool bStartActivation;

	UPROPERTY(BlueprintReadOnly, Category = "Activation with order")
	int32 ActivationsNum = 0;
	
public:
	AG2IValveWithActivationOrder();

	virtual void ApplyLocalRotation() override;

	// Activates component
	void Interact_Implementation(const ACharacter* Interactor) override;

	// Switches activation (same as interaction with AG2IValve)
	virtual void Activate_Implementation() override;

	// Switches to the start activation (e.g. Restore)
	virtual void Deactivate_Implementation() override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void KeepActivation();
};

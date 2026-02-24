#pragma once

#include "CoreMinimal.h"
#include "Gameplay/G2IValve.h"
#include "Components/BoxComponent.h"
#include "Components/G2IOwnerActivationOrderComponent.h"
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Activation with order|Components")
	TObjectPtr<UG2IOwnerActivationOrderComponent> ActivationOrderComponent;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Activation with order")
	bool bStartActivation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Activation with order")
	int32 ActivationsNum = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> TriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activation with order|Sound",
		meta = (Tooltip="Set of characters that can overlap with trigger box of this valve and generate sound."))
	TSet<TObjectPtr<ACharacter>> AllowedCharactersToOverlapSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activation with order|Sound")
	TObjectPtr<USoundBase> SoundOnOverlap;
	
public:
	AG2IValveWithActivationOrder();

	virtual void ApplyLocalRotation() override;

	// Activates component
	void Interact_Implementation(const ACharacter* Interactor) override;

	// Switches activation (same as interaction with AG2IValve)
	virtual void Activate_Implementation() override;

	// Restores start condition
	virtual void Deactivate_Implementation() override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void KeepActivation();

	UFUNCTION()
	void OnTriggerBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};

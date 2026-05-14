#pragma once

#include "CoreMinimal.h"
#include "Gameplay/G2IButtonActivator.h"
#include "G2IButtonAirShipActivator.generated.h"

UCLASS()
class G2I_API AG2IButtonAirShipActivator : public AG2IButtonActivator
{
	GENERATED_BODY()

public:
	AG2IButtonAirShipActivator();

	virtual bool CanInteract_Implementation(const ACharacter* Interactor) override;
	virtual void Interact_Implementation(const ACharacter* Interactor) override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float PushForce = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float PushDuration = 2.0f;

private:
	bool bIsPushing = false;
	float CurrentPushTime = 0.0f;
};
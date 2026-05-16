#pragma once

#include "CoreMinimal.h"
#include "Gameplay/G2IActivationOrderManager.h"
#include "G2IActivationOrderManagerSounds.generated.h"

/**
 * Activation Order Manager, plays sounds on order completion
 * & allows to cancel last activated actor
 */

class UG2ISoundComponent;

UCLASS()
class G2I_API AG2IActivationOrderManagerSounds : public AG2IActivationOrderManager
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activation with order")
	bool bAllowCancel = true;

	int32 CorrectSoundId = -1;
	int32 FailedSoundId = -1;

public:
	AG2IActivationOrderManagerSounds();

	virtual void OrderCompleted() override;

	virtual void OrderFailed() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CorrectSoundName = FName("CorrectSound");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FailedSoundName = FName("FailedSound");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<UG2ISoundComponent> SoundComp;

protected:
	virtual void BeginPlay() override;

	virtual void OnActorActivated(AActor* ActivatedActor, bool bReactivation, UG2IActivationWithOrderComponent* ActivationComponent) override;
};

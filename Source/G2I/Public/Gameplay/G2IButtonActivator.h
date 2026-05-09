#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/G2IInteractiveObjectInterface.h"
#include "G2IButtonActivator.generated.h"

class UG2ILauncherComponent;
class UG2ISoundComponent;

UCLASS()
class G2I_API AG2IButtonActivator : public AActor, public IG2IInteractiveObjectInterface
{
	GENERATED_BODY()

private:
	int32 ActivationSoundId = -1;

protected:

	UPROPERTY(EditAnywhere, Category = "Data|Activation")
	TSet<TObjectPtr<AActor>> ActorsToActivate;

	UPROPERTY(EditAnywhere, Category = "Data")
	bool bLockOnInteraction = true;

	virtual void BeginPlay() override;

public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<UG2ISoundComponent> SoundComp;

	AG2IButtonActivator();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ActivationSoundName = FName("ActivationSound");

	UPROPERTY(EditAnywhere)
	TObjectPtr<UG2IWorldHintKeyWidgetComponent> HintKeyWidgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UG2ILauncherComponent> LauncherComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRootComponent;

	virtual bool CanInteract_Implementation(const ACharacter* Interactor) override;
	virtual void Interact_Implementation(const ACharacter* Interactor) override;
	virtual UG2IWorldHintKeyWidgetComponent* GetInteractionKeyHintComponent_Implementation() override;
};

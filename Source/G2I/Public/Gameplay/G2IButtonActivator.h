#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/G2IInteractiveObjectInterface.h"
#include "G2IButtonActivator.generated.h"

class UG2ILauncherComponent;

UCLASS()
class G2I_API AG2IButtonActivator : public AActor, public IG2IInteractiveObjectInterface
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, Category = "Data|Activation")
	TSet<TObjectPtr<AActor>> ActorsToActivate;

	virtual void BeginPlay() override;

public:	
	AG2IButtonActivator();

	UPROPERTY(EditAnywhere)
	TObjectPtr<UG2IWorldHintKeyWidgetComponent> HintKeyWidgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UG2ILauncherComponent> LauncherComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	virtual bool CanInteract_Implementation(const ACharacter* Interactor) override;
	virtual void Interact_Implementation(const ACharacter* Interactor) override;
	virtual UG2IWorldHintKeyWidgetComponent* GetInteractionKeyHintComponent_Implementation() override;
};

#pragma once

#include "CoreMinimal.h"
#include "G2IInteractiveObjectInterface.h"
#include "GameFramework/Actor.h"
#include "G2ITransitBetweenLevelThroughInteraction.generated.h"

class UG2ILauncherComponent;
class UG2ITransitBetweenLevelsComponent;

UCLASS()
class G2I_API AG2ITransitBetweenLevelThroughInteraction : public AActor, public IG2IInteractiveObjectInterface
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UG2IWorldHintKeyWidgetComponent> HintKeyWidgetComp;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UG2ILauncherComponent> LauncherComp;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UG2ITransitBetweenLevelsComponent> TransitComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	
public:

	AG2ITransitBetweenLevelThroughInteraction();

	virtual bool CanInteract_Implementation(const ACharacter* Interactor) override;
	virtual void Interact_Implementation(const ACharacter* Interactor) override;
	virtual UG2IWorldHintKeyWidgetComponent *GetInteractionKeyHintComponent_Implementation() override;
	
protected:
	
	virtual void BeginPlay() override;
	
};

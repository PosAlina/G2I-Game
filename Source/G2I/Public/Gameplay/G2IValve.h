
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Interfaces/G2IInteractiveObjectInterface.h"
#include "G2IValve.generated.h"

UCLASS()
class G2I_API AG2IValve : public AActor, public IG2IInteractiveObjectInterface
{
	GENERATED_BODY()
	
public:	
	AG2IValve();

	void BeginPlay() override;

	// Only characters with UG2IValveInteractionComponent can interact
	bool CanInteract_Implementation(const ACharacter* Interactor) override;

	// Basic implementation works only for pipes,
	// override it if you want to extend the execution
	void Interact_Implementation(const ACharacter* Interactor) override;

	UFUNCTION()
	void PassActivationToPipe();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Valve")
	bool bActivated;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Valve")
	TObjectPtr<AActor> OwnerActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
};

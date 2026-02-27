
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

	virtual void Tick(float DeltaTime) override;

	// Only characters with UG2IValveInteractionComponent can interact
	bool CanInteract_Implementation(const ACharacter* Interactor) override;

	// Basic implementation works only for pipes,
	// override it if you want to extend the execution
	void Interact_Implementation(const ACharacter* Interactor) override;

	UFUNCTION()
	void PassActivationToPipe();

protected:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual void ApplyLocalRotation();

	UFUNCTION(BlueprintCallable)
	virtual void ChangeActivation();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Valve")
	bool bActivated;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Valve")
	TObjectPtr<AActor> OwnerActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	// Used for animation
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Valve|Animation")
	FRotator DeltaRotation = FRotator(5., 0., 0.);

	// Used for animation
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Valve|Animation")
	FRotator MaxRotation = FRotator(90., 0., 0.);

	// Used for animation
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Valve|Animation")
	FRotator MinRotation = FRotator::ZeroRotator;

protected:

	UPROPERTY(VisibleAnywhere, Category = "Valve|Animation")
	FRotator CurrentRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRootComponent;
};

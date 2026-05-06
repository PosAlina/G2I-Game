#pragma once

#include "CoreMinimal.h"
#include "G2ILockingInterface.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Interfaces/G2IInteractiveObjectInterface.h"
#include "G2IValve.generated.h"

class UG2ISoundComponent;

UCLASS()
class G2I_API AG2IValve : public AActor, public IG2IInteractiveObjectInterface, public IG2ILockingInterface
{
	GENERATED_BODY()
	
public:	
	static inline const FName ValveRotationSoundName = FName("SliderMoveSound");

	AG2IValve();

	virtual void Tick(float DeltaTime) override;

	// Only characters with UG2IValveInteractionComponent can interact
	virtual bool CanInteract_Implementation(const ACharacter* Interactor) override;

	// Basic implementation works only for pipes,
	// override it if you want to extend the execution
	virtual void Interact_Implementation(const ACharacter* Interactor) override;

	virtual UG2IWorldHintKeyWidgetComponent *GetInteractionKeyHintComponent_Implementation() override;

	virtual void SetIsLocked_Implementation(bool bIsNewLocked) override;
	virtual bool IsLocked_Implementation() override;

	UFUNCTION()
	void PassActivationToPipe();

	void AddRotationToStaticMesh(const FRotator& Rotation) const;

protected:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual void ApplyLocalRotation();

	UFUNCTION(BlueprintCallable)
	virtual void ChangeActivation();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<UG2ISoundComponent> SoundComp;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UG2IWorldHintKeyWidgetComponent> HintKeyWidgetComp;

protected:

	UPROPERTY(VisibleAnywhere, Category = "Valve|Animation")
	FRotator CurrentRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRootComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsLocked = false;
	
	int32 ValveRotationSoundId = -1;
};

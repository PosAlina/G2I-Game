#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "G2IPlayerController.generated.h"

class UG2ICameraDefaultsParameters;
class UCameraComponent;
struct FInputActionInstance;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;

/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(abstract)
class G2I_API AG2IPlayerController : public APlayerController
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UG2ICameraDefaultsParameters> CameraDefaultsParameters;
	
public:

	virtual void OnPossess(APawn *NewPawn) override;

public:

	void SetRotationTowardsCamera(const UCameraComponent& Camera);

	TObjectPtr<UG2ICameraDefaultsParameters> GetCameraDefaultsParameters();

protected:

	/** Setup Input */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<TObjectPtr<UInputMappingContext>> InputMappingContexts;
	
	virtual void SetupInputComponent() override;

	void SetupCharacterActorComponents();

	void SetupCamera();

	/** Actions */
	/** Select camera actions */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UActorComponent> CameraControllersComponent;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> SwitchCameraBehaviorAction;
	
	void SwitchCameraBehavior();

	UPROPERTY(VisibleAnywhere)
	TSet<TObjectPtr<UActorComponent>> ThirdPersonCameraComponents;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> LookAction;

	void Look(const FInputActionValue& Value);

	UPROPERTY(VisibleAnywhere)
	TSet<TObjectPtr<UActorComponent>> MovementComponents;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;
	
	void Move(const FInputActionValue& Value);
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> JumpAction;

	void Jump(const FInputActionValue& Value);
	
	void StopJumping(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ToggleCrouchAction;

	void ToggleCrouch(const FInputActionValue& Value);

	/** Select character actions */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> SelectNextCharacterAction;

	void SelectNextCharacter(const FInputActionValue& Value);

	/** Interact Actions */
	UPROPERTY(EditAnywhere, Category = "Input")
	TSet<TObjectPtr<UInputAction>> InteractActions;

	UPROPERTY(VisibleAnywhere)
	TSet<TObjectPtr<UActorComponent>> InteractionComponents;

	void Interact(const FInputActionInstance& Instance);

	UPROPERTY(EditAnywhere, Category = "Data")
	TMap<TObjectPtr<UInputAction>, FName> ActionToTagMap;
};
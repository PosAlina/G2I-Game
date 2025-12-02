#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "G2IPlayerController.generated.h"

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

public:

	virtual void OnPossess(APawn *NewPawn) override;

protected:

	/** Setup Input */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> InputMappingContexts;
	
	virtual void SetupInputComponent() override;

	void SetupCharacterActorComponents();

	/** Actions */
	UPROPERTY(VisibleAnywhere)
	TSet<UActorComponent*> CameraComponents;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	void Look(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;
	
	void MouseLook(const FInputActionValue& Value);

	UPROPERTY(VisibleAnywhere)
	TSet<UActorComponent*> MovementComponents;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;
	
	void Move(const FInputActionValue& Value);
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	void Jump(const FInputActionValue& Value);
	
	void StopJumping(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* ToggleCrouchAction;

	void ToggleCrouch(const FInputActionValue& Value);

	/** Select character actions */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* SelectNextCharacterAction;

	void SelectNextCharacter(const FInputActionValue& Value);

	/** Interact Actions */
	UPROPERTY(EditAnywhere, Category = "Input")
	TSet<UInputAction*> InteractActions;

	UPROPERTY(VisibleAnywhere)
	TSet<UActorComponent*> InteractionComponents;

	void Interact(const FInputActionInstance& Instance);

	UPROPERTY(EditAnywhere, Category = "Data")
	TMap<UInputAction*, FName> ActionToTagMap;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* GadgetAction;

	UPROPERTY(VisibleAnywhere)
	TSet<UActorComponent*> GadgetComponents;
	
	void GadgetActivation(const FInputActionInstance& Instance);
};

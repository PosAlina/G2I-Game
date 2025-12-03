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
	TArray<TObjectPtr<UInputMappingContext>> InputMappingContexts;
	
	virtual void SetupInputComponent() override;

	void SetupCharacterActorComponents();

	/** Actions */
	UPROPERTY(VisibleAnywhere)
	TSet<TObjectPtr<UActorComponent>> CameraComponents;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> LookAction;

	void Look(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MouseLookAction;
	
	void MouseLook(const FInputActionValue& Value);

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

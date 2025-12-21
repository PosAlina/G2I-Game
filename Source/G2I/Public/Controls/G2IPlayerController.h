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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnPossessPawnDelegate, APawn *, Pawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPossessPawnDelegate, APawn *, Pawn);

/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(abstract)
class G2I_API AG2IPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FUnPossessPawnDelegate OnUnPossessPawnDelegate;

	UPROPERTY(BlueprintAssignable)
	FPossessPawnDelegate OnPossessPawnDelegate;
	
private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UG2ICameraDefaultsParameters> CameraDefaultsParameters;
	
public:

	AG2IPlayerController();

	virtual void OnPossess(APawn *NewPawn) override;

	virtual void OnUnPossess() override;

	virtual void SetViewTargetWithBlend(AActor* NewViewTarget, float BlendTime = 0,
		EViewTargetBlendFunction BlendFunc = VTBlend_Linear, float BlendExp = 0, bool bLockOutgoing = false) override;

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
	TObjectPtr<UActorComponent> MovementComponent;
	
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

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UActorComponent> SteamMovementComponent;
};
#pragma once

#include "CoreMinimal.h"
#include "Components/G2ICameraComponent.h"
#include "Components/G2IMovementComponent.h"
#include "GameFramework/Character.h"
#include "Interfaces/G2IReactToInputInterface.h"
#include "Logging/LogMacros.h"
#include <Components/SphereComponent.h>
#include "G2ICharacterEngineer.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

/**
 *  First player`s character. Engineer.
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class G2I_API AG2ICharacterEngineer : public ACharacter, public IG2IReactToInputInterface
{
	GENERATED_BODY()

public:

	/** Constructor */
	AG2ICharacterEngineer();

	/** TODO: Refactor CameraComponent */
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UG2ICameraComponent *CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UG2IMovementComponent *MovementComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	float InteractionSphereRadius = 200.f;

private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** Interaction Sphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* InteractionSphere;

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Returns InteractionSphere subobject **/
	FORCEINLINE class USphereComponent* GetInteractionSphere() const { return InteractionSphere; }

	

	/** TODO: Refactor MovementComponent */
	
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void MoveAction_Implementation(const float Right, const float Forward, const FRotator Rotation) override;

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void LookAction_Implementation(const float Yaw, const float Pitch) override;

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void MouseLookAction_Implementation(const float Yaw, const float Pitch) override;
	
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void JumpAction_Implementation() override;

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void StopJumpingAction_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void InteractAction_Implementation(const FName& Tag) override;

};


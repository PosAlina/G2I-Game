#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/G2IReactToInputInterface.h"
#include <Components/SphereComponent.h>
#include "G2ICharacterDaughter.generated.h"

class UG2ICameraComponent;
class UG2IMovementComponent;
class USpringArmComponent;
class UCameraComponent;

/**
 *  Second player`s character. Daughter.
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class AG2ICharacterDaughter : public ACharacter, public IG2IReactToInputInterface
{
	GENERATED_BODY()

public:

	/** Constructor */
	AG2ICharacterDaughter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UG2ICameraComponent *CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UG2IMovementComponent *MovementComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	float InteractionSphereRadius = 200.f;

	/** TODO: Refactor CameraComponent */
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

protected:

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void LookAction_Implementation(const float Yaw, const float Pitch) override;

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void MouseLookAction_Implementation(const float Yaw, const float Pitch) override;

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void InteractAction_Implementation(const FName& Tag) override;

};


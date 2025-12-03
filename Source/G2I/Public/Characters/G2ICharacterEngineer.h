#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "G2ICharacterEngineer.generated.h"

class UG2ICharacterCollisionComponent;
class UG2ICharacterMovementComponent;
class UG2ICameraComponent;
class UG2IInteractionComponent;

/**
 *  First player`s character. Engineer.
 *  Implements a controllable orbiting camera
 */
UCLASS(Blueprintable)
class G2I_API AG2ICharacterEngineer : public ACharacter
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UG2ICharacterCollisionComponent> CollisionComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UG2ICameraComponent> CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UG2ICharacterMovementComponent> MovementComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UG2IInteractionComponent> InteractionComp;

public:

	AG2ICharacterEngineer();

};


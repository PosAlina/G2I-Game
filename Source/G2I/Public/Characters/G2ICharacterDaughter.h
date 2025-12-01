#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "G2ICharacterDaughter.generated.h"

class UG2ICharacterCollisionComponent;
class UG2IPassingThroughObjectsComponent;
class UG2ICameraComponent;
class UG2IInteractionComponent;
class UG2ICharacterMovementComponent;

/**
 *  Second player`s character. Daughter.
 *  Implements a controllable orbiting camera
 */
UCLASS(Blueprintable)
class AG2ICharacterDaughter : public ACharacter
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	UG2ICharacterCollisionComponent *CollisionComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UG2ICameraComponent *CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UG2ICharacterMovementComponent *MovementComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	UG2IInteractionComponent* InteractionComp;

public:

	AG2ICharacterDaughter();
	
};


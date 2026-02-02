#pragma once

#include "CoreMinimal.h"
#include "G2ICharacterInterface.h"
#include "GameFramework/Character.h"
#include "G2ICharacterDaughter.generated.h"

class UG2IFixedCamerasComponent;
class UG2ICameraControllerComponent;
class UG2ICharacterCollisionComponent;
class UG2IPassingThroughObjectsComponent;
class UG2IThirdPersonCameraComponent;
class UG2IInteractionComponent;

/**
 *  Second player`s character. Daughter.
 *  Implements a controllable orbiting camera
 */
UCLASS(Blueprintable)
class AG2ICharacterDaughter : public ACharacter, public IG2ICharacterInterface
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UG2ICharacterCollisionComponent> CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UG2IInteractionComponent> InteractionComp;
	
	/** Different behavior of camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr<UG2ICameraControllerComponent> CameraControllerComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr<UG2IThirdPersonCameraComponent> ThirdPersonCameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr<UG2IFixedCamerasComponent> FixedCamerasComp;

private:

	UPROPERTY(BlueprintAssignable)
	FPossessedDelegate OnPossessedDelegate;

	UPROPERTY(BlueprintAssignable)
	FUnPossessedDelegate OnUnPossessedDelegate;

public:

	explicit AG2ICharacterDaughter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PossessedBy(AController* NewController) override;
	
	virtual void UnPossessed() override;
	
	UFUNCTION()
	virtual FPossessedDelegate& GetPossessedDelegate() override;

	UFUNCTION()
	virtual FUnPossessedDelegate& GetUnPossessedDelegate() override;
	
};


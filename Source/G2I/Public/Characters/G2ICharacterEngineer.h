#pragma once

#include "CoreMinimal.h"
#include "G2ICharacterInterface.h"
#include "GameFramework/Character.h"
#include "Components/G2IValveInteractionComponent.h"
#include "Components/G2IHoleInteractionComponent.h"
#include "G2ICharacterEngineer.generated.h"

class UG2ISteamGloveComponent;
class UG2IFixedCamerasComponent;
class UG2IThirdPersonCameraComponent;
class UG2ICameraControllerComponent;
class UG2ICharacterCollisionComponent;
class UG2IInteractionComponent;
class UG2IInventoryComponent;

/**
 *  First player`s character. Engineer.
 *  Implements a controllable orbiting camera
 */
UCLASS(Blueprintable)
class G2I_API AG2ICharacterEngineer : public ACharacter, public IG2ICharacterInterface
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UG2ICharacterCollisionComponent> CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UG2IInteractionComponent> InteractionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UG2IValveInteractionComponent> ValveInteractionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UG2IHoleInteractionComponent> HoleInteractionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gadgets)
	TObjectPtr<UG2ISteamGloveComponent> SteamGloveComp;
	
	/** Different behavior of camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr<UG2ICameraControllerComponent> CameraControllerComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr<UG2IThirdPersonCameraComponent> ThirdPersonCameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr<UG2IFixedCamerasComponent> FixedCamerasComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	UG2IInventoryComponent* InventoryComp;


private:

	UPROPERTY(BlueprintAssignable)
	FPossessedDelegate OnPossessedDelegate;

	UPROPERTY(BlueprintAssignable)
	FUnPossessedDelegate OnUnPossessedDelegate;

public:

	explicit AG2ICharacterEngineer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PossessedBy(AController* NewController) override;
	
	virtual void UnPossessed() override;
	
	UFUNCTION()
	virtual FPossessedDelegate& GetPossessedDelegate() override;

	UFUNCTION()
	virtual FUnPossessedDelegate& GetUnPossessedDelegate() override;
	
};


#include "G2ICharacterEngineer.h"
#include "Components/Camera/G2IThirdPersonCameraComponent.h"
#include "Components/G2ICharacterCollisionComponent.h"
#include "Components/G2ICharacterMovementComponent.h"
#include "Components/G2IInteractionComponent.h"
#include "Components/Camera/G2ICameraControllerComponent.h"
#include "Components/Camera/G2IFixedCamerasComponent.h"
#include "Engine/World.h"

AG2ICharacterEngineer::AG2ICharacterEngineer(const FObjectInitializer& ObjectInitializer)
	: ACharacter(ObjectInitializer.SetDefaultSubobjectClass<UG2ICharacterMovementComponent>(
		CharacterMovementComponentName))
{
	CollisionComp = CreateDefaultSubobject<UG2ICharacterCollisionComponent>(TEXT("CollisionComp"));
	InteractionComp = CreateDefaultSubobject<UG2IInteractionComponent>(TEXT("InteractionComp"));
	CameraControllerComp = CreateDefaultSubobject<UG2ICameraControllerComponent>(TEXT("CameraControllerComp"));
	ThirdPersonCameraComp = CreateDefaultSubobject<UG2IThirdPersonCameraComponent>(TEXT("ThirdPersonCameraComp"));
	FixedCamerasComp = CreateDefaultSubobject<UG2IFixedCamerasComponent>(TEXT("FixedCamerasComp"));
}

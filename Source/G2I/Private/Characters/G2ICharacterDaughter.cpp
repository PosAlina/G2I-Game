#include "G2ICharacterDaughter.h"

#include "G2I.h"
#include "Engine/LocalPlayer.h"
#include "Components/Camera/G2IThirdPersonCameraComponent.h"
#include "Components/G2ICharacterMovementComponent.h"
#include "Components/G2IInteractionComponent.h"
#include "Components/G2ICharacterCollisionComponent.h"
#include "Components/Camera/G2ICameraControllerComponent.h"
#include "Components/Camera/G2IFixedCamerasComponent.h"

AG2ICharacterDaughter::AG2ICharacterDaughter()
{
	CollisionComp = CreateDefaultSubobject<UG2ICharacterCollisionComponent>(TEXT("CollisionComp"));
	MovementComp = CreateDefaultSubobject<UG2ICharacterMovementComponent>(TEXT("MovementComp"));
	InteractionComp = CreateDefaultSubobject<UG2IInteractionComponent>(TEXT("InteractionComp"));

	CameraControllerComp = CreateDefaultSubobject<UG2ICameraControllerComponent>(TEXT("CameraControllerComp"));
	ThirdPersonCameraComp = CreateDefaultSubobject<UG2IThirdPersonCameraComponent>(TEXT("ThirdPersonCameraComp"));
	FixedCamerasComp = CreateDefaultSubobject<UG2IFixedCamerasComponent>(TEXT("FixedCamerasComp"));

	if (!ensure(MovementComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s hasn't movement component"), *GetName());
		return;
	}

	MovementComp->SetCanPassThroughObject(true);
}

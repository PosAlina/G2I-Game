#include "G2ICharacterDaughter.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Controller.h"
#include "Components/G2ICameraComponent.h"
#include "Components/G2ICharacterMovementComponent.h"
#include "Components/G2IInteractionComponent.h"
#include "Components/G2ICharacterCollisionComponent.h"

AG2ICharacterDaughter::AG2ICharacterDaughter()
{
	CollisionComp = CreateDefaultSubobject<UG2ICharacterCollisionComponent>(TEXT("CollisionComp"));
	CameraComp = CreateDefaultSubobject<UG2ICameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(RootComponent);
	MovementComp = CreateDefaultSubobject<UG2ICharacterMovementComponent>(TEXT("MovementComp"));
	InteractionComp = CreateDefaultSubobject<UG2IInteractionComponent>(FName("InteractionComp"));
		
	MovementComp->SetCanPassThroughObject(true);
}

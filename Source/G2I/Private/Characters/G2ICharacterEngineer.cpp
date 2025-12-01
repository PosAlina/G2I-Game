#include "G2ICharacterEngineer.h"
#include "Engine/LocalPlayer.h"
#include "Components/G2ICameraComponent.h"
#include "Components/G2ICharacterCollisionComponent.h"
#include "Components/G2ICharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Components/G2IInteractionComponent.h"

AG2ICharacterEngineer::AG2ICharacterEngineer()
{
	CollisionComp = CreateDefaultSubobject<UG2ICharacterCollisionComponent>(TEXT("CollisionComp"));
	CameraComp = CreateDefaultSubobject<UG2ICameraComponent>(FName("CameraComp"));
	CameraComp->SetupAttachment(RootComponent);
	MovementComp = CreateDefaultSubobject<UG2ICharacterMovementComponent>(FName("MovementComp"));
	InteractionComp = CreateDefaultSubobject<UG2IInteractionComponent>(FName("InteractionComp"));
}
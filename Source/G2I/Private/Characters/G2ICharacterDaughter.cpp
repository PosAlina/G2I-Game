#include "G2ICharacterDaughter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "Components/G2ICameraComponent.h"
#include "Components/G2ICharacterMovementComponent.h"
#include "Components/G2IInteractionComponent.h"
#include "Components/G2ICharacterCollisionComponent.h"

AG2ICharacterDaughter::AG2ICharacterDaughter()
{
	CollisionComp = CreateDefaultSubobject<UG2ICharacterCollisionComponent>(TEXT("CollisionComp"));
	CameraComp = CreateDefaultSubobject<UG2ICameraComponent>(TEXT("CameraComp"));
	MovementComp = CreateDefaultSubobject<UG2ICharacterMovementComponent>(TEXT("MovementComp"));
	InteractionComp = CreateDefaultSubobject<UG2IInteractionComponent>(FName("InteractionComp"));
		
	MovementComp->SetCanPassThroughObject(true);
		
	/** TODO: Refactor to Camera Component */
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	SetupPassingThroughObjectCamera(*CameraBoom);
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AG2ICharacterDaughter::LookAction_Implementation(const float Yaw, const float Pitch)
{
	// add yaw and pitch input to controller
	AddControllerYawInput(Yaw);
	AddControllerPitchInput(Pitch);
}

void AG2ICharacterDaughter::MouseLookAction_Implementation(const float Yaw, const float Pitch)
{
	// add yaw and pitch input to controller
	AddControllerYawInput(Yaw);
	AddControllerPitchInput(Pitch);
}

void AG2ICharacterDaughter::SetupPassingThroughObjectCamera(USpringArmComponent& Camera)
{
	if (UActorComponent *Component = GetComponentByClass(UG2ICharacterMovementComponent::StaticClass()))
	{
		if (const UG2ICharacterMovementComponent *MovementComponent = Cast<UG2ICharacterMovementComponent>(Component))
		{
			if (MovementComponent->CanPassThroughObject())
			{
				Camera.bDoCollisionTest = false;
			}
		}
	}
}

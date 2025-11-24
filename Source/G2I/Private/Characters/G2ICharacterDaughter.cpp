#include "G2ICharacterDaughter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "G2IInteractiveObjectInterface.h"
#include "Components/G2ICameraComponent.h"
#include "Components/G2IMovementComponent.h"

AG2ICharacterDaughter::AG2ICharacterDaughter()
{
	CameraComp = CreateDefaultSubobject<UG2ICameraComponent>(TEXT("CameraComp"));
	MovementComp = CreateDefaultSubobject<UG2IMovementComponent>(TEXT("MovementComp"));

	/** TODO: Refactor to Component */
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
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

	// Create a interaction sphere
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(RootComponent);
	InteractionSphere->InitSphereRadius(InteractionSphereRadius);
	InteractionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

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
	
void AG2ICharacterDaughter::JumpAction_Implementation()
{
	Jump();
}

void AG2ICharacterDaughter::StopJumpingAction_Implementation()
{
	StopJumping();
}

void AG2ICharacterDaughter::InteractAction_Implementation(const FName& Tag)
{
	DrawDebugSphere(
		GetWorld(),
		InteractionSphere->GetComponentLocation(),
		InteractionSphere->GetScaledSphereRadius(),
		16,
		FColor::Green,
		false,
		1.0f,
		0,
		2.0f
	);

	TArray<AActor*> OverlappedActors;
	InteractionSphere->GetOverlappingActors(OverlappedActors);

	for (const auto& Overlap : OverlappedActors) {
		if (Overlap->ActorHasTag(Tag)) {
			if (Overlap->Implements<UG2IInteractiveObjectInterface>()) {
				if (IG2IInteractiveObjectInterface::Execute_CanInteract(Overlap, this)) {
					IG2IInteractiveObjectInterface::Execute_Interact(Overlap, this);
				}
			}
		}
	}
}
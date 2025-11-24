#include "G2ICharacterEngineer.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/G2ICameraComponent.h"
#include "Components/G2IMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "G2IInteractiveObjectInterface.h"
#include "G2I.h"

AG2ICharacterEngineer::AG2ICharacterEngineer()
{
	CameraComp = CreateDefaultSubobject<UG2ICameraComponent>(FName("CameraComp"));
	MovementComp = CreateDefaultSubobject<UG2IMovementComponent>(FName("MovementComp"));

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

void AG2ICharacterEngineer::LookAction_Implementation(const float Yaw, const float Pitch)
{
	// add yaw and pitch input to controller
	AddControllerYawInput(Yaw);
	AddControllerPitchInput(Pitch);
}

void AG2ICharacterEngineer::MouseLookAction_Implementation(const float Yaw, const float Pitch)
{
	// add yaw and pitch input to controller
	AddControllerYawInput(Yaw);
	AddControllerPitchInput(Pitch);
}
	
void AG2ICharacterEngineer::JumpAction_Implementation()
{
	Jump();
}

void AG2ICharacterEngineer::StopJumpingAction_Implementation()
{
	StopJumping();
}

void AG2ICharacterEngineer::InteractAction_Implementation(const FName& Tag)
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

	UE_LOG(LogTemp, Log, TEXT("InteractAction: found %d overlapping actors"), OverlappedActors.Num());

	for (AActor* Overlap : OverlappedActors)
	{
		if (!Overlap)
		{
			UE_LOG(LogTemp, Log, TEXT("Overlap actor is null!"));
			continue;
		}

		UE_LOG(LogTemp, Log, TEXT("Checking actor: %s"), *Overlap->GetName());

		if (Overlap->ActorHasTag(Tag))
		{
			UE_LOG(LogTemp, Log, TEXT("Actor %s has tag %s"), *Overlap->GetName(), *Tag.ToString());

			if (Overlap->Implements<UG2IInteractiveObjectInterface>())
			{
				UE_LOG(LogTemp, Log, TEXT("Actor %s implements IG2IInteractiveObjectInterface"), *Overlap->GetName());
				
				if (IG2IInteractiveObjectInterface::Execute_CanInteract(Overlap, this))
				{
					UE_LOG(LogTemp, Log, TEXT("Actor %s CAN interact, executing Interact"), *Overlap->GetName());
					IG2IInteractiveObjectInterface::Execute_Interact(Overlap, this);
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("Actor %s CANNOT interact"), *Overlap->GetName());
				}
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Actor %s does NOT implement IG2IInteractiveObjectInterface"), *Overlap->GetName());
			}
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Actor %s does NOT have tag %s"), *Overlap->GetName(), *Tag.ToString());
		}
	}
}

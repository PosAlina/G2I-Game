#include "Components/G2ICameraComponent.h"
#include "G2I.h"
#include "Camera/CameraComponent.h"
#include "Components/G2ICharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"

UG2ICameraComponent::UG2ICameraComponent()
{
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->bUsePawnControlRotation = false;
}

void UG2ICameraComponent::OnRegister()
{
	Super::OnRegister();

	if (ACharacter *Owner = Cast<ACharacter>(GetOwner()))
	{
		CameraBoom->SetupAttachment(this);
		FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
		
		// Don't rotate when the controller rotates. Let that just affect the camera.
		Owner->bUseControllerRotationPitch = false;
		Owner->bUseControllerRotationYaw = false;
		Owner->bUseControllerRotationRoll = false;

		if (CameraBoom)
		{
			SetupPassingThroughObjectCamera(*Owner, *CameraBoom);
		}
		else
		{
			UE_LOG(LogG2I, Warning, TEXT("Spring arm component doesn't exist in %s"), *GetName());
		}
	}
}


void UG2ICameraComponent::LookAction_Implementation(const float Yaw, const float Pitch)
{
	if (ACharacter *Owner = Cast<ACharacter>(GetOwner()))
	{
		// add yaw and pitch input to controller
		Owner->AddControllerYawInput(Yaw);
		Owner->AddControllerPitchInput(Pitch);
	}
}

void UG2ICameraComponent::MouseLookAction_Implementation(const float Yaw, const float Pitch)
{
	if (ACharacter *Owner = Cast<ACharacter>(GetOwner()))
	{
		// add yaw and pitch input to controller
		Owner->AddControllerYawInput(Yaw);
		Owner->AddControllerPitchInput(Pitch);
	}
}

void UG2ICameraComponent::SetupPassingThroughObjectCamera(const ACharacter& Owner, USpringArmComponent& Camera)
{
	if (UActorComponent *Component = Owner.GetComponentByClass(UG2ICharacterMovementComponent::StaticClass()))
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
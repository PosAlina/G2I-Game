#include "Components/Camera/G2IThirdPersonCameraComponent.h"
#include "G2I.h"
#include "Camera/CameraComponent.h"
#include "Camera/G2ICameraControllerInputInterface.h"
#include "Components/G2ICharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"

UG2IThirdPersonCameraComponent::UG2IThirdPersonCameraComponent()
{
	// Create a camera boom (pulls in towards the player if there is a collision)
	ThirdPersonCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("ThirdPersonCameraBoom"));
	ThirdPersonCameraBoom->TargetArmLength = 400.0f;
	ThirdPersonCameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	ThirdPersonFollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonFollowCamera"));
	ThirdPersonFollowCamera->bUsePawnControlRotation = false;
}

void UG2IThirdPersonCameraComponent::OnRegister()
{
	Super::OnRegister();

	Owner = Cast<ACharacter>(GetOwner());
	if (Owner)
	{
		ThirdPersonCameraBoom->SetupAttachment(Owner->GetRootComponent());
		ThirdPersonFollowCamera->SetupAttachment(ThirdPersonCameraBoom, USpringArmComponent::SocketName);
		
		// Don't rotate when the controller rotates. Let that just affect the camera.
		Owner->bUseControllerRotationPitch = false;
		Owner->bUseControllerRotationYaw = false;
		Owner->bUseControllerRotationRoll = false;

		if (ThirdPersonCameraBoom)
		{
			SetupPassingThroughObjectCamera(*ThirdPersonCameraBoom);
		}
		else
		{
			UE_LOG(LogG2I, Warning, TEXT("Spring arm component doesn't exist in %s"), *GetName());
		}
	}
}

void UG2IThirdPersonCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Owner)
	{
		if (CameraController = Owner->FindComponentByInterface(UG2ICameraControllerInputInterface::StaticClass()))
		{
			UE_LOG(LogG2I, Log, TEXT("%s have camera controller class"), *Owner->GetName());
		}
		else
		{
			UE_LOG(LogG2I, Error, TEXT("%s have not camera controller class"), *Owner->GetName());
		}
	}
}

void UG2IThirdPersonCameraComponent::SetActive(bool bNewActive, bool bReset)
{
	if (Owner = Cast<ACharacter>(GetOwner()))
	{
		// Don't rotate when the controller rotates. Let that just affect the camera.
		Owner->bUseControllerRotationPitch = false;
		Owner->bUseControllerRotationYaw = false;
		Owner->bUseControllerRotationRoll = false;

	}
	
	Super::SetActive(bNewActive, bReset);
}

UCameraComponent* UG2IThirdPersonCameraComponent::GetCameraComponent_Implementation() const
{
	return ThirdPersonFollowCamera;
}


void UG2IThirdPersonCameraComponent::LookAction_Implementation(const float Yaw)
{
	if (CameraController)
	{
		if (IG2ICameraControllerInputInterface::Execute_GetCameraComponent(CameraController) == GetCameraComponent_Implementation())
		{
			if (Owner)
			{
				Owner->AddControllerYawInput(Yaw);
			}
		}
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s have not camera controller class"), *Owner->GetName());
	}
}

void UG2IThirdPersonCameraComponent::SetupPassingThroughObjectCamera(USpringArmComponent& Camera)
{
	if (UActorComponent *Component = Owner->GetComponentByClass(UG2ICharacterMovementComponent::StaticClass()))
	{
		if (const UG2ICharacterMovementComponent *MovementComponent = Cast<UG2ICharacterMovementComponent>(Component))
		{
			if (MovementComponent->CanPassThroughObject())
			{
				//Camera.bDoCollisionTest = false;
			}
		}
	}
}
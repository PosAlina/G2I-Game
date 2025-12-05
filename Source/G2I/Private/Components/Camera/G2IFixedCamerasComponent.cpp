#include "Components/Camera/G2IFixedCamerasComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/G2ICharacterCollisionComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"

UCameraComponent* UG2IFixedCamerasComponent::GetCameraComponent_Implementation() const
{
	return FollowCamera;
}

UG2IFixedCamerasComponent::UG2IFixedCamerasComponent()
{
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->TargetArmLength = 600.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->bUsePawnControlRotation = false;
}

void UG2IFixedCamerasComponent::BeginPlay()
{
	Super::BeginPlay();

	BindingToDelegates();
}

void UG2IFixedCamerasComponent::OnRegister()
{
	Super::OnRegister();

	if (ACharacter *Owner = Cast<ACharacter>(GetOwner()))
	{
		CameraBoom->SetupAttachment(Owner->GetRootComponent());
		FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
		
		// Don't rotate when the controller rotates. Let that just affect the camera.
		Owner->bUseControllerRotationPitch = false;
		Owner->bUseControllerRotationYaw = false;
		Owner->bUseControllerRotationRoll = false;

	}
}

void UG2IFixedCamerasComponent::BindingToDelegates()
{
	if (ACharacter *Owner = Cast<ACharacter>(GetOwner()))
	{
		if (UG2ICharacterCollisionComponent *CollisionComponent =
			Owner->FindComponentByClass<UG2ICharacterCollisionComponent>())
		{
			//CollisionComponent->OnCharacterCollisionOverlap.AddDynamic(this, )
		}
	}
}

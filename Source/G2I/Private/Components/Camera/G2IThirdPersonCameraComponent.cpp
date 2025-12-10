#include "Components/Camera/G2IThirdPersonCameraComponent.h"
#include "G2I.h"
#include "Camera/CameraComponent.h"
#include "Camera/G2ICameraControllerInputInterface.h"
#include "Components/G2ICharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"

UG2IThirdPersonCameraComponent::UG2IThirdPersonCameraComponent()
{
	InitializeCameraBoom();
	InitializeFollowCamera();
}

void UG2IThirdPersonCameraComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SetupDefaults();
}

void UG2IThirdPersonCameraComponent::OnRegister()
{
	Super::OnRegister();

	PreInitializeDefaults();
}

TArray<UCameraComponent *> UG2IThirdPersonCameraComponent::GetCameraComponents_Implementation() const
{
	return{ThirdPersonFollowCamera};
}

void UG2IThirdPersonCameraComponent::SetupCameras_Implementation()
{
	OnAddCameraDelegate.Broadcast(ThirdPersonFollowCamera);
}

FAddCameraDelegate& UG2IThirdPersonCameraComponent::GetAddCameraDelegate()
{
	return OnAddCameraDelegate;
}

FRemoveCameraDelegate& UG2IThirdPersonCameraComponent::GetRemoveCameraDelegate()
{
	return OnRemoveCameraDelegate;
}

void UG2IThirdPersonCameraComponent::LookAction_Implementation(const float Yaw)
{
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner isn't character in %s"), *GetName());
		return;
	}
	
	if (CameraController)
	{
		if (IG2ICameraControllerInputInterface::Execute_GetCameraComponent(CameraController) !=
			ThirdPersonFollowCamera)
		{
			return;
		}
	}
	
	Owner->AddControllerYawInput(Yaw);
}

void UG2IThirdPersonCameraComponent::SetupDefaults()
{
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner isn't character in %s"), *GetName());
		return;
	}

	CameraController = Owner->FindComponentByInterface(UG2ICameraControllerInputInterface::StaticClass());
	if (!CameraController)
	{
		UE_LOG(LogG2I, Log, TEXT("%s hasn't camera controller in %s"), *Owner->GetName(), *GetName());
	}
	else
	{
		UE_LOG(LogG2I, Log, TEXT("%s has camera controller %s in %s"), *Owner->GetName(),
			*CameraController->GetName(), *GetName());
	}
}

void UG2IThirdPersonCameraComponent::PreInitializeDefaults()
{
	AActor *OwnerActor = GetOwner();
	if (!ensure(OwnerActor))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner doesn't exist in %s"), *GetName());
		return;
	}
	
	Owner = Cast<ACharacter>(OwnerActor);
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner isn't character in %s"), *GetName());
		return;
	}

	if (!ensure(ThirdPersonCameraBoom))
	{
		UE_LOG(LogG2I, Error, TEXT("Spring arm isn't exist in %s"), *GetName());
		return;
	}
	ThirdPersonCameraBoom->SetupAttachment(Owner->GetRootComponent());

	if (!ensure(ThirdPersonFollowCamera))
	{
		UE_LOG(LogG2I, Error, TEXT("Follow camera isn't exist in %s"), *GetName());
		return;
	}
	ThirdPersonFollowCamera->SetupAttachment(ThirdPersonCameraBoom, USpringArmComponent::SocketName);
}

void UG2IThirdPersonCameraComponent::InitializeCameraBoom()
{
	ThirdPersonCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("ThirdPersonCameraBoom"));
	if (!ensure(ThirdPersonCameraBoom))
	{
		UE_LOG(LogG2I, Error, TEXT("Spring arm isn't exist in %s"), *GetName());
		return;
	}
	
	ThirdPersonCameraBoom->TargetArmLength = 400.0f;
	ThirdPersonCameraBoom->bUsePawnControlRotation = true;
}

void UG2IThirdPersonCameraComponent::InitializeFollowCamera()
{
	ThirdPersonFollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonFollowCamera"));
	if (!ensure(ThirdPersonFollowCamera))
	{
		UE_LOG(LogG2I, Error, TEXT("Follow camera isn't exist in %s"), *GetName());
		return;
	}
	
	ThirdPersonFollowCamera->bUsePawnControlRotation = false;
}
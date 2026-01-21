#include "G2IThirdPersonCameraComponent.h"
#include "G2I.h"
#include "G2IAimingComponent.h"
#include "G2ICameraDefaultsParameters.h"
#include "G2IPlayerController.h"
#include "Camera/CameraComponent.h"
#include "G2ICameraControllerInputInterface.h"
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
	BindDelegates();
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

void UG2IThirdPersonCameraComponent::LookAction_Implementation(const float Yaw, const float Pitch)
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

	const float TargetYaw = CameraDefaultsParameters->bIsInvertedCameraHorizontalRotation ? -Yaw : Yaw;
	Owner->AddControllerYawInput(TargetYaw);

	const float TargetPitch = CameraDefaultsParameters->bIsInvertedCameraVerticalRotation ? Pitch : -Pitch;
	Owner->AddControllerPitchInput(TargetPitch);
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

	const UWorld *World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}

	APlayerController *LocalPlayerController = World->GetFirstPlayerController();
	if (!ensure(LocalPlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Local player controller doesn't exist in %s"), *GetName());
		return;
	}

	AG2IPlayerController *PlayerController = Cast<AG2IPlayerController>(LocalPlayerController);
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Player Controller doesn't exist in %s"), *GetName());
		return;
	}
	
	CameraDefaultsParameters = PlayerController->GetCameraDefaultsParameters();
	if (!ensure(CameraDefaultsParameters))
	{
		UE_LOG(LogG2I, Error, TEXT("%s can not return camera defaults parameters in %s"),
			*PlayerController->GetName(), *GetName());
		return;
	}
}

void UG2IThirdPersonCameraComponent::BindDelegates()
{
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner isn't character in %s"), *GetName());
		return;
	}

	if (UG2IAimingComponent *AimingComponent = Owner->FindComponentByClass<UG2IAimingComponent>())
	{
		AimingComponent->OnStartAimingDelegate.AddDynamic(this, &ThisClass::SetAimCameraLocation);
		AimingComponent->OnFinishAimingDelegate.AddDynamic(this, &ThisClass::SetDefaultCameraLocation);
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

	ThirdPersonCameraBoom->TargetArmLength = 220.0f;
	ThirdPersonCameraBoom->SocketOffset = FVector(0.f, 80.f, 70.f);
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

void UG2IThirdPersonCameraComponent::SetDefaultCameraLocation()
{
	if (!ensure(ThirdPersonCameraBoom))
	{
		UE_LOG(LogG2I, Error, TEXT("Spring arm isn't exist in %s"), *GetName());
		return;
	}
	
	if (CameraDefaultsParameters)
	{
		ThirdPersonCameraBoom->TargetArmLength = CameraDefaultsParameters->DefaultTargetArm;
		ThirdPersonCameraBoom->SocketOffset = FVector(0, CameraDefaultsParameters->DefaultHorizontalOffset, CameraDefaultsParameters->DefaultVerticalOffset);
	}
	else
	{
		ThirdPersonCameraBoom->TargetArmLength = 220.0f;
		ThirdPersonCameraBoom->SocketOffset = FVector(0.f, 80.f, 70.f);
	}
}

void UG2IThirdPersonCameraComponent::SetAimCameraLocation()
{
	if (!ensure(ThirdPersonCameraBoom))
	{
		UE_LOG(LogG2I, Error, TEXT("Spring arm isn't exist in %s"), *GetName());
		return;
	}
	
	if (CameraDefaultsParameters)
	{
		ThirdPersonCameraBoom->TargetArmLength = CameraDefaultsParameters->AimTargetArm;
		ThirdPersonCameraBoom->SocketOffset = FVector(0, CameraDefaultsParameters->AimHorizontalOffset, CameraDefaultsParameters->AimVerticalOffset);
	}
	else
	{
		ThirdPersonCameraBoom->TargetArmLength = 160.0f;
		ThirdPersonCameraBoom->SocketOffset = FVector(0.f, 80.f, 70.f);
	}
}
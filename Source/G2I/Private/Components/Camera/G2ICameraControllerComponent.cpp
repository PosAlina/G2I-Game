#include "Components/Camera/G2ICameraControllerComponent.h"
#include "G2I.h"
#include "G2ICameraDefaultsParameters.h"
#include "G2IPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Camera/G2ICameraInterface.h"
#include "Camera/G2IFixedCamerasInputInterface.h"
#include "Camera/G2IThirdPersonCameraInputInterface.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"

void UG2ICameraControllerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	PrimaryComponentTick.bCanEverTick = true;
	SetupDefaults();
	BindDelegates();
	SetupCamerasDefaults();
}

void UG2ICameraControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsOwnerControllable())
	{
		if (!CurrentCameraComponents.IsEmpty() &&
			CurrentCameraComponents[CurrentCameraIndex % CurrentCameraComponents.Num()])
		{
			PlayerController->SetRotationTowardsCamera(
				*CurrentCameraComponents[CurrentCameraIndex % CurrentCameraComponents.Num()]);
		}
	}
}

void UG2ICameraControllerComponent::SetupCurrentCamera_Implementation()
{
	SetCurrentCamera(CurrentCameraIndex);
}

void UG2ICameraControllerComponent::SwitchCameraBehavior_Implementation()
{
	for (int32 CameraOffset = 1; CameraOffset < CurrentCameraComponents.Num(); ++CameraOffset)
	{
		const int32 NewCameraIndex = (CurrentCameraIndex + CameraOffset) % CurrentCameraComponents.Num();
		if (SetCurrentCamera(NewCameraIndex))
		{
			return;
		}
	}
	
	UE_LOG(LogG2I, Log, TEXT("Camera did not switch in %s"), *GetName());
}

UCameraComponent* UG2ICameraControllerComponent::GetCameraComponent_Implementation() const
{
	if (!CurrentCameraComponents.IsEmpty())
	{
		return CurrentCameraComponents[CurrentCameraIndex % CurrentCameraComponents.Num()];
	}
	return nullptr;
}

void UG2ICameraControllerComponent::AddCamera(UCameraComponent* AddedCamera)
{
	if (!AddedCamera)
	{
		UE_LOG(LogG2I, Warning, TEXT("Attempt to add nullptr camera in %s"), *GetName());
		return;
	}
	
	if (CurrentCameraComponents.Contains(AddedCamera))
	{
		return;
	}
	
	CurrentCameraComponents.Add(AddedCamera);
	if (CurrentCameraComponents.Num() > 1)
	{
		return;
	}

	// Set current camera
	CurrentCameraIndex = 0;
	if (!IsOwnerControllable())
	{
		return;
	}
	if (!SetCamera(*AddedCamera))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s can not become current camera in %s"), *AddedCamera->GetName(),
			*GetName());
	}
}

void UG2ICameraControllerComponent::RemoveCamera(UCameraComponent* RemovedCamera)
{
	if (CurrentCameraComponents.Num() <= 1)
	{
		UE_LOG(LogG2I, Warning, TEXT("Attempt to remove last or nonexistent camera in %s"), *GetName());
		return;
	}
	
	if (!ensure(RemovedCamera))
	{
		UE_LOG(LogG2I, Error, TEXT("Attempt to remove nullptr camera in %s"), *GetName());
		return;
	}
	
	if (RemovedCamera == CurrentCameraComponents[CurrentCameraIndex % CurrentCameraComponents.Num()])
	{
		const uint32 NewCameraComponentIndex = (CurrentCameraIndex + 1) % CurrentCameraComponents.Num();
		const UCameraComponent *NewCamera = CurrentCameraComponents[NewCameraComponentIndex];
		if (!ensure(NewCamera))
		{
			UE_LOG(LogG2I, Error, TEXT("Current camera's array has nullptr camera in %i place in %s"),
				NewCameraComponentIndex, *GetName());
			return;
		}
		
		bool bCameraChanged = true;
		if (IsOwnerControllable())
		{
			bCameraChanged = SetCamera(*NewCamera);
		}
		if (bCameraChanged)
		{
			CurrentCameraComponents.RemoveAt(CurrentCameraIndex % CurrentCameraComponents.Num());
			CurrentCameraIndex %= CurrentCameraComponents.Num();
		}
	}
	else
	{
		const int32 RemovedCameraIndex = CurrentCameraComponents.Find(RemovedCamera);
		if (CurrentCameraIndex > RemovedCameraIndex)
		{
			CurrentCameraIndex = (CurrentCameraIndex - 1) % CurrentCameraComponents.Num();
		}
		CurrentCameraComponents.RemoveAt(RemovedCameraIndex);
	}
}

bool UG2ICameraControllerComponent::IsOwnerControllable() const
{
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner doesn't exist in %s"), *GetName());
		return false;
	}
	
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Player Controller doesn't exist in %s"), *GetName());
		return false;
	}
	
	if (Owner != PlayerController->GetPawn())
	{
		return false;
	}
	return true;
}


bool UG2ICameraControllerComponent::SetCamera(const UCameraComponent& NewCamera) const
{
	AActor *OwnerActor = NewCamera.GetOwner();
	if (!OwnerActor)
	{
		UE_LOG(LogG2I, Warning, TEXT("%s doesn't have owner"), *NewCamera.GetName());
		return false;
	}

	UE_LOG(LogG2I, Log, TEXT("%s - camera component loaded in %s"), *NewCamera.GetName(), *Owner->GetName());
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Player Controller doesn't exist in %s"), *GetName());
		return false;
	}
	
	PlayerController->SetViewTargetWithBlend(OwnerActor, CameraDefaultsParameters->CameraTransitionTime);
	PlayerController->SetRotationTowardsCamera(NewCamera);
	return true;
}


bool UG2ICameraControllerComponent::SetCurrentCamera(int32 NewCameraIndex)
{
	if (!IsOwnerControllable())
	{
		return false;
	}
	
	if (CurrentCameraComponents.IsEmpty())
	{
		return false;
	}
	
	NewCameraIndex %= CurrentCameraComponents.Num();
	const UCameraComponent *NewCameraComponent = CurrentCameraComponents[NewCameraIndex];
	if (!ensure(NewCameraComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("Current camera's array has nullptr camera in %i place in %s"),
			NewCameraIndex, *GetName());
		return false;
	}
	
	if (SetCamera(*NewCameraComponent))
	{
		CurrentCameraIndex = NewCameraIndex;
		return true;
	}
	
	return false;
}

void UG2ICameraControllerComponent::SetupDefaults()
{
	const UWorld *World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}
	
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
	
	APlayerController *LocalPlayerController = World->GetFirstPlayerController();
	if (!ensure(LocalPlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Local player controller doesn't exist in %s"), *GetName());
		return;
	}
	
	PlayerController = Cast<AG2IPlayerController>(LocalPlayerController);
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

void UG2ICameraControllerComponent::BindDelegates()
{
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner doesn't exist in %s"), *GetName());
		return;
	}
	
	TArray<UActorComponent *> ActorCameraComponents =
		Owner->GetComponentsByInterface(UG2ICameraInterface::StaticClass());
	for (UActorComponent *ActorCameraComponent : ActorCameraComponents)
	{
		if (IG2ICameraInterface *CameraComponent = Cast<IG2ICameraInterface>(ActorCameraComponent))
		{
			CameraComponent->GetAddCameraDelegate().AddDynamic(this,
				&ThisClass::UG2ICameraControllerComponent::AddCamera);
			CameraComponent->GetRemoveCameraDelegate().AddDynamic(this,
				&ThisClass::UG2ICameraControllerComponent::RemoveCamera);
		}
	}
}

void UG2ICameraControllerComponent::SetupCamerasDefaults()
{
	SetupThirdPersonCameras();
	if (!CurrentCameraComponents.IsEmpty())
	{
		CurrentCameraIndex = CurrentCameraComponents.Num() - 1;
	}
	
	SetupFixedCameras();
	// Current camera is fixed, if fixed camera exists
	if (CurrentCameraIndex < CurrentCameraComponents.Num() - 1)
	{
		++CurrentCameraIndex;
	}

	SetupCurrentCamera_Implementation();
}

void UG2ICameraControllerComponent::SetupThirdPersonCameras() const
{
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner doesn't exist in %s"), *GetName());
		return;
	}
	
	for (UActorComponent *ActorCameraComponent :
		Owner->GetComponentsByInterface(UG2IThirdPersonCameraInputInterface::StaticClass()))
	{
		if (ActorCameraComponent->Implements<UG2ICameraInterface>())
		{
			IG2ICameraInterface::Execute_SetupCameras(ActorCameraComponent);
		}
	}
}

void UG2ICameraControllerComponent::SetupFixedCameras() const
{
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner doesn't exist in %s"), *GetName());
		return;
	}
	
	for (UActorComponent *ActorCameraComponent :
		Owner->GetComponentsByInterface(UG2IFixedCamerasInputInterface::StaticClass()))
	{
		if (ActorCameraComponent->Implements<UG2ICameraInterface>())
		{
			IG2ICameraInterface::Execute_SetupCameras(ActorCameraComponent);
		}
	}
}

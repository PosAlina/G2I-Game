#include "Components/Camera/G2ICameraControllerComponent.h"

#include "G2I.h"
#include "Camera/CameraComponent.h"
#include "Camera/G2ICameraInterface.h"
#include "Components/G2IInteractionComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"

void UG2ICameraControllerComponent::BeginPlay()
{
	Super::BeginPlay();

	SetupCameraControllersDefaults();
}

void UG2ICameraControllerComponent::SetupCameraControllersDefaults()
{
	SetupPlayerController();
	SetupCameraComponents_Implementation();
}

void UG2ICameraControllerComponent::SetupPlayerController()
{
	if (const UWorld *World = GetWorld())
	{
		PlayerController = World->GetFirstPlayerController();
		if (PlayerController == nullptr)
		{
			UE_LOG(LogG2I, Error, TEXT("Player Controller doesn't exist"));
		}
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist"));
	}
}

void UG2ICameraControllerComponent::SetupCameraComponents_Implementation()
{
	if (const ACharacter *Owner = Cast<ACharacter>(GetOwner()))
	{
		CameraComponents = Owner->GetComponentsByInterface(UG2ICameraInterface::StaticClass());
		if (Owner == PlayerController->GetPawn())
		{
			SetNextCameraWithStartIndex(CurrentCameraIndex);
		}
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s doesn't have owner"), *GetName());
	}
}

UCameraComponent* UG2ICameraControllerComponent::GetCameraComponent_Implementation() const
{
	return CurrentCameraComponent;
}

void UG2ICameraControllerComponent::SetNextCameraWithStartIndex(const int32 CameraIndex)
{
	if (!CameraComponents.IsEmpty())
	{
		for (int32 CameraOffset = 0; CameraOffset < CameraComponents.Num(); ++CameraOffset)
		{
			const int32 NewCameraIndex = (CameraIndex + CameraOffset) % CameraComponents.Num();
			if (SetCurrentCamera(NewCameraIndex))
			{
				return;
			}
		}
		UE_LOG(LogG2I, Error, TEXT("There is not any camera in all camera components"));
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("There is not any camerat"));
	}
}

bool UG2ICameraControllerComponent::SetCurrentCamera(const int32 CameraIndex)
{
	if (const UActorComponent *CameraActorComponent = CameraComponents[CameraIndex])
	{
		if (CameraActorComponent->Implements<UG2ICameraInterface>())
		{
			if (UCameraComponent* NewCameraComponent = IG2ICameraInterface::Execute_GetCameraComponent(
				CameraActorComponent))
			{
				if (AActor *OwnerActor = NewCameraComponent->GetOwner())
				{
					if (APawn *Owner = Cast<APawn>(OwnerActor))
					{
						UE_LOG(LogG2I, Log, TEXT("%s - camera component loaded in %s"),
							*NewCameraComponent->GetName(), *Owner->GetName());

						if (CurrentCameraComponent)
						{
							CurrentCameraComponent->SetActive(false);
						}
						CurrentCameraComponent = NewCameraComponent;
						CurrentCameraComponent->SetActive(true);
						PlayerController->SetViewTargetWithBlend(OwnerActor, BlendTime);
						
						return true;
					}
				}
				else
				{
					UE_LOG(LogG2I, Warning, TEXT("%s doesn't have owner"), *NewCameraComponent->GetName());
				}
			}
			else
			{
				UE_LOG(LogG2I, Error, TEXT("%s doesn't have camera component"),
					*CameraActorComponent->GetName());
			}
		}
	}
	return false;
}

void UG2ICameraControllerComponent::SwitchCameraBehavior_Implementation()
{
	CurrentCameraIndex = (CurrentCameraIndex + 1) % CameraComponents.Num();
	SetNextCameraWithStartIndex(CurrentCameraIndex);
}

#include "Components/Camera/G2IFixedCamerasComponent.h"
#include "G2I.h"
#include "G2IFixedCameraActor.h"
#include "Camera/CameraComponent.h"
#include "Components/G2ICharacterCollisionComponent.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"

void UG2IFixedCamerasComponent::BeginPlay()
{
	Super::BeginPlay();

	BindingToDelegates();
}

void UG2IFixedCamerasComponent::OnRegister()
{
	Super::OnRegister();
	
	PreInitializationDefaults();
}

TArray<UCameraComponent*> UG2IFixedCamerasComponent::GetCameraComponents_Implementation() const
{
	return CurrentFixedCameras;
}

void UG2IFixedCamerasComponent::SetupCameras_Implementation()
{
	if (!ensure(CollisionComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("Collision component doesn't exist in %s"),	*GetName());
		return;
	}
	
	const UPrimitiveComponent *CharacterCollision = CollisionComponent->GetCollisionComponent();
	if (!ensure(CharacterCollision))
	{
		UE_LOG(LogG2I, Error, TEXT("Collision component in %s doesn't exist in %s"),
			*CollisionComponent->GetName(), *GetName());
		return;
	}
	
	TArray<AActor*> OverlappingActors;
	CharacterCollision->GetOverlappingActors(OverlappingActors);
	for (AActor *OverlappingActor : OverlappingActors)
	{
		AG2IFixedCameraActor *FixedCameraActor = Cast<AG2IFixedCameraActor>(OverlappingActor);
		if (!FixedCameraActor)
		{
			continue;
		}
		
		const UBoxComponent *ZoneTrigger = FixedCameraActor->GetFixedCameraZoneTrigger();
		if (!ZoneTrigger)
		{
			continue;
		}
		
		if (CharacterCollision->IsOverlappingComponent(ZoneTrigger))
		{
			UCameraComponent *FixedCamera = FixedCameraActor->GetFixedCamera();
			if (!FixedCamera)
			{
				continue;
			}
			
			AddCamera(*FixedCamera);
		}
	}
}

FAddCameraDelegate& UG2IFixedCamerasComponent::GetAddCameraDelegate()
{
	return OnAddCameraDelegate;
}

FRemoveCameraDelegate& UG2IFixedCamerasComponent::GetRemoveCameraDelegate()
{
	return OnRemoveCameraDelegate;
}

void UG2IFixedCamerasComponent::OnCharacterBeginOverlapTriggerZone(AActor *OtherActor, UPrimitiveComponent *OtherComp)
{
	AG2IFixedCameraActor *FixedCameraActor = Cast<AG2IFixedCameraActor>(OtherActor);
	if (!FixedCameraActor)
	{
		return;
	}
	
	if (OtherComp != FixedCameraActor->GetFixedCameraZoneTrigger())
	{
		return;
	}
	
	UCameraComponent *FixedCamera = FixedCameraActor->GetFixedCamera();
	if (!FixedCamera)
	{
		return;
	}
	
	AddCamera(*FixedCamera);
}

void UG2IFixedCamerasComponent::OnCharacterEndOverlapTriggerZone(AActor *OtherActor, UPrimitiveComponent *OtherComp)
{
	AG2IFixedCameraActor *FixedCameraActor = Cast<AG2IFixedCameraActor>(OtherActor);
	if (!FixedCameraActor)
	{
		return;
	}
	
	if (OtherComp != FixedCameraActor->GetFixedCameraZoneTrigger())
	{
		return;
	}

	UCameraComponent *FixedCamera = FixedCameraActor->GetFixedCamera();
	if (!FixedCamera)
	{
		return;
	}

	RemovedCamera(*FixedCamera);
}

void UG2IFixedCamerasComponent::PreInitializationDefaults()
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
	
	CollisionComponent = Owner->FindComponentByClass<UG2ICharacterCollisionComponent>();
	if (!ensure(CollisionComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner %s has not collision component in %s"), *Owner->GetName(),
			*GetName());
		return;
	}
}

void UG2IFixedCamerasComponent::BindingToDelegates()
{
	if (!ensure(CollisionComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("Collision component doesn't exist in %s"),	*GetName());
		return;
	}
	
	CollisionComponent->OnCharacterCollisionBeginOverlap.AddDynamic(this,
		&ThisClass::OnCharacterBeginOverlapTriggerZone);
	CollisionComponent->OnCharacterCollisionEndOverlap.AddDynamic(this,
		&ThisClass::OnCharacterEndOverlapTriggerZone);
}

void UG2IFixedCamerasComponent::AddCamera(UCameraComponent& AddedCamera)
{
	if (CurrentFixedCameras.Contains(&AddedCamera))
	{
		return;
	}
	
	CurrentFixedCameras.Add(&AddedCamera);
	OnAddCameraDelegate.Broadcast(&AddedCamera);
}

void UG2IFixedCamerasComponent::RemovedCamera(UCameraComponent& RemovedCamera)
{
	const int32 RemovedCameraIndex = CurrentFixedCameras.Find(&RemovedCamera);
	if (RemovedCameraIndex == INDEX_NONE)
	{
		return;
	}
	
	CurrentFixedCameras.RemoveAt(RemovedCameraIndex);
	OnRemoveCameraDelegate.Broadcast(&RemovedCamera);
}
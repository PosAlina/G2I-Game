#include "G2IFixedCameraActor.h"
#include "G2I.h"
#include "Components/BoxComponent.h"
#include "Components/G2ICharacterCollisionComponent.h"

AG2IFixedCameraActor::AG2IFixedCameraActor()
{
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(FName("Root")));
	InitializeFixedCameraZoneTrigger();
	InitializeFixedCamera();
}

UBoxComponent *AG2IFixedCameraActor::GetFixedCameraZoneTrigger()
{
	return FixedCameraZoneTrigger;
}

UCameraComponent* AG2IFixedCameraActor::GetFixedCamera()
{
	return FixedCamera;
}

void AG2IFixedCameraActor::InitializeFixedCameraZoneTrigger()
{
	FixedCameraZoneTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger zone"));
	if (!ensure(FixedCameraZoneTrigger))
	{
		UE_LOG(LogG2I, Error, TEXT("There is not any camera in all camera components %s"), *GetName());
		return;
	}
	
	FixedCameraZoneTrigger->SetupAttachment(RootComponent);
	FixedCameraZoneTrigger->SetRelativeScale3D(FVector(10.0f, 10.0f, 10.0f));
	FixedCameraZoneTrigger->SetCollisionResponseToChannel(
		UG2ICharacterCollisionComponent::GetPassingThroughCollisionChannel(), ECR_Overlap);
}

void AG2IFixedCameraActor::InitializeFixedCamera()
{
	FixedCamera = CreateDefaultSubobject<UCineCameraComponent>(TEXT("Fixed camera"));
	if (!ensure(FixedCamera))
	{
		UE_LOG(LogG2I, Error, TEXT("There is not any camera in all camera components %s"), *GetName());
		return;
	}
	
	FixedCamera->SetupAttachment(RootComponent);
	FixedCamera->SetRelativeLocation(FVector(-900.0f, 0.0f, 400.0f));
	FixedCamera->SetRelativeRotation(FRotator(-30, 0, 0));
}

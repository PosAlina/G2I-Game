#include "Gameplay/G2IRotatingPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "G2I.h"

AG2IRotatingPlatform::AG2IRotatingPlatform()
{
	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	RootComponent = PlatformMesh;

	RotatingComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingComponent"));
	if (!ensure(RotatingComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create RotatingComponent"), *GetActorNameOrLabel());
	}
	RotatingComponent->RotationRate = FRotator(0.f, 60.f, 0.f);
}

void AG2IRotatingPlatform::BeginPlay()
{
	Super::BeginPlay();

	if (!ensure(RotatingComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("RotatingComponent is NULL %s"), *GetActorNameOrLabel());
		return;
	}

	RotatingComponent->RotationRate = RotationSpeed;
}

void AG2IRotatingPlatform::SetRotationSpeed(const FRotator NewRotationSpeed)
{
	RotationSpeed = NewRotationSpeed;

	if (ensure(RotatingComponent))
	{
		RotatingComponent->RotationRate = RotationSpeed;
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("RotatingComponent is NULL %s"), *GetActorNameOrLabel());
	}
}

void AG2IRotatingPlatform::Activate_Implementation()
{
	bShouldRotate = true;

	if (!ensure(RotatingComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("RotatingComponent is NULL %s"), *GetActorNameOrLabel());
	}

	RotatingComponent->Activate(true);
}

void AG2IRotatingPlatform::Deactivate_Implementation()
{
	bShouldRotate = false;

	if (!ensure(RotatingComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("RotatingComponent is NULL %s"), *GetActorNameOrLabel());
	}

	RotatingComponent->Deactivate();
}
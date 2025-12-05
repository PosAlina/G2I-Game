#include "G2IFixedCameraActor.h"

#include "Engine/TriggerBox.h"

void AG2IFixedCameraActor::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;

	FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 100.f);
	FRotator SpawnRotation = FRotator::ZeroRotator;

	if (UWorld *World = GetWorld())
	{
		FixedCameraZoneTrigger = World->SpawnActor<ATriggerBox>(ATriggerBox::StaticClass(), SpawnLocation,
			SpawnRotation, SpawnParameters);

		FixedCamera = World->SpawnActor<ACineCameraActor>(ATriggerBox::StaticClass(), SpawnLocation,
			SpawnRotation, SpawnParameters);
	}
}

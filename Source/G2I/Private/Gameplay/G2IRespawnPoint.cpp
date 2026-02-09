
#include "Gameplay/G2IRespawnPoint.h"

// Sets default values
AG2IRespawnPoint::AG2IRespawnPoint()
{
    RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("DirectionArrow"));

	if (!ensure(RootSceneComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("Failed to create RootSceneComponent in %s"), *GetActorNameOrLabel());
		return;
	}

	SetRootComponent(RootSceneComponent);

	if (ensure(ArrowComponent))
		ArrowComponent->SetupAttachment(RootComponent);
	else
		UE_LOG(LogG2I, Error, TEXT("Failed to create ArrowComponent in %s"), *GetActorNameOrLabel());
}

void AG2IRespawnPoint::BeginPlay()
{
	Super::BeginPlay();
}

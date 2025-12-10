#include "G2IInteractableActorBase.h"


// Sets default values
AG2IInteractableActorBase::AG2IInteractableActorBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AG2IInteractableActorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AG2IInteractableActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}





#include "Gameplay/G2IDestructibleActorBase.h"

// Sets default values
AG2IDestructibleActorBase::AG2IDestructibleActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AG2IDestructibleActorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AG2IDestructibleActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AG2IDestructibleActorBase::DestroySelf_Implementation()
{
	this->Destroy();
}


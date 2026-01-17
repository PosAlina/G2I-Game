#include "Gameplay/G2IDestructibleActorBase.h"

// Sets default values
AG2IDestructibleActorBase::AG2IDestructibleActorBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AG2IDestructibleActorBase::DestroySelf_Implementation()
{
	this->Destroy();
}


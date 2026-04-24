#include "Gameplay/G2IDestructibleObjectsManager.h"
#include "G2I.h"
#include "G2IDestructibleActorBase.h"

void AG2IDestructibleObjectsManager::BeginPlay()
{
	DestructibleObjNumber = DestructibleObjectPool.Num();

	if (!ensure(ActorToDestroy))
	{
		UE_LOG(LogG2I, Warning, TEXT("ActorToDestroy is null in %s"), *GetName());
		return;
	}

	for (const auto DestructibleObj : DestructibleObjectPool)
	{
		DestructibleObj->OnDestroyedDelegate.BindUObject(this, &ThisClass::DestroyedObjectCounter);
	}
}

void AG2IDestructibleObjectsManager::DestroyedObjectCounter()
{
	DestroyedObjCurrentNumber++;

	if (DestroyedObjCurrentNumber == DestructibleObjNumber)
	{
		if (!ensure(ActorToDestroy))
		{
			UE_LOG(LogG2I, Warning, TEXT("ActorToDestroy is null in %s"), *GetName());
			return;
		}
		
		ActorToDestroy->Destroy();
	}
}

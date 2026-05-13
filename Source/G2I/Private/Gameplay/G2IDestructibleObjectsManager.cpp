#include "Gameplay/G2IDestructibleObjectsManager.h"
#include "G2I.h"
#include "G2IDestructibleActorBase.h"
#include "G2ISoundComponent.h"

AG2IDestructibleObjectsManager::AG2IDestructibleObjectsManager() {
	SoundComp = CreateDefaultSubobject<UG2ISoundComponent>(TEXT("SoundComponent"));
	if (SoundComp) {
		SoundComp->SetupSounds.Add(DestroySoundName, FSoundConfig());
	}
	else {
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create %s"), *GetActorNameOrLabel(),
			*UG2ISoundComponent::StaticClass()->GetName());
	}
}

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
		DestructibleObj->OnDestroyedDelegate.AddUObject(this, &ThisClass::DestroyedObjectCounter);
	}

	if (SoundComp) {
		if (const auto* SoundConf = SoundComp->SetupSounds.Find(DestroySoundName)) {
			DestroySoundId = SoundComp->AddSound(*SoundConf);
		}
		else {
			UE_LOG(LogG2I, Error, TEXT("%s: sound config is null"), *GetActorNameOrLabel());
		}
	}
	else {
		UE_LOG(LogG2I, Error, TEXT("%s: %s is null"), *GetActorNameOrLabel(),
			*UG2ISoundComponent::StaticClass()->GetName());
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
		
		UG2ISoundComponent::PlaySoundSafe(SoundComp, DestroySoundId);
		ActorToDestroy->Destroy();
	}
}

#include "Game/G2IGameSoundManager.h"
#define StartSize 20
#include "G2I.h"
#include "Components/AudioComponent.h"

void AG2IGameSoundManager::BeginPlay()
{
	for (int32 i = StartSize - 1; i >= 0; i--) {
		IdStack.Push(i);
	}
}

int AG2IGameSoundManager::AddSound(USoundCue* NewSound)
{
	if (!NewSound) {
		UE_LOG(LogG2I, Warning, TEXT("The sound manager didn't get sound"));
		return -1;
	}

	UAudioComponent* AudioComponent = NewObject<UAudioComponent>();
	if (!AudioComponent) {
		UE_LOG(LogG2I, Warning, TEXT("The sound manager can't create an audio component"));
		return -1;
	}
	AudioComponent->SetSound(NewSound);
	AudioComponent->RegisterComponent();

	if (USceneComponent* Scene = GetRootComponent())
	{
		AudioComponent->AttachToComponent(
			Scene,
			FAttachmentTransformRules::KeepRelativeTransform
		);
	}
	else {
		UE_LOG(LogG2I, Warning, TEXT("Sound manager can't get the scene root component"));
	}

	int32 NewSoundId = IdStack.Pop();
	ActiveSounds.Add(NewSoundId, AudioComponent);
	AudioComponent->Play();

	return NewSoundId;
}

bool AG2IGameSoundManager::RemoveSound(int32 SoundId)
{
	if (SoundId < 0) {
		UE_LOG(LogG2I, Warning, TEXT("Sound manager got wrong SoundId"));
		return false;
	}
	UAudioComponent* Component = *ActiveSounds.Find(SoundId);
	if (!Component) {
		return false;
	}
	Component->Stop();
	Component->DestroyComponent();
	ActiveSounds.Remove(SoundId);

	IdStack.Push(SoundId);
	return true;
}

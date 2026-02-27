#include "Game/G2IGameSoundManager.h"
#define STARTSIZE 20
#include "G2I.h"
#include "Components/AudioComponent.h"

TObjectPtr<UAudioComponent> UG2IGameSoundManager::GetAudioById(int32 SoundId) noexcept
{
	if (SoundId < 0) {
		UE_LOG(LogG2I, Warning, TEXT("Sound manager got wrong SoundId"));
		return TObjectPtr<UAudioComponent>();
	}

	return *ActiveSounds.Find(SoundId);
}

TObjectPtr<UG2IGameSoundManager> UG2IGameSoundManager::Get(UObject* WorldContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull))
	{
		return World->GetSubsystem<UG2IGameSoundManager>();
	}

	return nullptr;
}

void UG2IGameSoundManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	for (int32 i = 0; i < STARTSIZE; ++i)
	{
		IdStack.Push(i);
	}
}

void UG2IGameSoundManager::Deinitialize()
{
	StopAllSounds();
	IdStack.Empty();
	ActiveSounds.Empty();

	Super::Deinitialize();
}

int32 UG2IGameSoundManager::AddSound(TObjectPtr<FSoundConfig> NewSoundConfig)
{
	if (!ensure(NewSoundConfig)) {
		UE_LOG(LogG2I, Warning, TEXT("The sound manager didn't get sound"));
		return -1;
	}
	if (!ensure(NewSoundConfig->Sound)) {
		UE_LOG(LogG2I, Warning, TEXT("The sound manager didn't get sound"));
		return -1;
	}
	TObjectPtr<UAudioComponent> AudioComponent = NewObject<UAudioComponent>();
	if (!ensure(AudioComponent)) {
		UE_LOG(LogG2I, Warning, TEXT("The sound manager can't create an audio component"));
		return -1;
	}

	AudioComponent->RegisterComponent();
	AudioComponent->SetSound(NewSoundConfig->Sound);
	AudioComponent->SetWorldLocation(NewSoundConfig->WorldLocation);

	if (NewSoundConfig->AttachToComponent) {
		AudioComponent->AttachToComponent(
			NewSoundConfig->AttachToComponent,
			NewSoundConfig->AttachmentRules);
	}

	AudioComponent->SetVolumeMultiplier(NewSoundConfig->VolumeMultiplier);
	AudioComponent->SetPitchMultiplier(NewSoundConfig->PitchMultiplier);

	int32 NewSoundId = IdStack.Pop();
	ActiveSounds.Add(NewSoundId, AudioComponent);

	return NewSoundId;
}

bool UG2IGameSoundManager::PlaySound(int32 SoundId)
{
	TObjectPtr<UAudioComponent> Component = GetAudioById(SoundId);
	if (!ensure(Component)) {
		UE_LOG(LogG2I, Warning, TEXT("The sound manager can't get an audio component"));
		return false;
	}

	Component->Play();

	return true;
}

bool UG2IGameSoundManager::StopSound(int32 SoundId)
{
	TObjectPtr<UAudioComponent> Component = GetAudioById(SoundId);
	if (!ensure(Component)) {
		UE_LOG(LogG2I, Warning, TEXT("The sound manager can't get an audio component"));
		return false;
	}

	Component->Stop();

	return true;
}

void UG2IGameSoundManager::StopAllSounds()
{
	for (const auto& Pair : ActiveSounds)
	{
		if (ensure(Pair.Value))
		{
			Pair.Value->Stop();
		}
	}
}

bool UG2IGameSoundManager::ChangeSoundVolume(int32 SoundId, float NewVolume)
{
	TObjectPtr<UAudioComponent> Component = GetAudioById(SoundId);
	if (!ensure(Component)) {
		UE_LOG(LogG2I, Warning, TEXT("The sound manager can't get an audio component"));
		return false;
	}

	NewVolume = FMath::Clamp(NewVolume, 0.0f, 1.0f);
	Component->SetVolumeMultiplier(NewVolume);
	
	return true;
}

bool UG2IGameSoundManager::ChangeSoundPitch(int32 SoundId, float NewPitch)
{
	TObjectPtr<UAudioComponent> Component = GetAudioById(SoundId);
	if (!ensure(Component)) {
		UE_LOG(LogG2I, Warning, TEXT("The sound manager can't get an audio component"));
		return false;
	}

	NewPitch = FMath::Clamp(NewPitch, 0.0f, 2.0f);
	Component->SetPitchMultiplier(NewPitch);
	
	return true;
}

bool UG2IGameSoundManager::ChangeSoundLocation(int32 SoundId, FVector NewLocation)
{
	TObjectPtr<UAudioComponent> Component = GetAudioById(SoundId);
	if (!ensure(Component)) {
		UE_LOG(LogG2I, Warning, TEXT("The sound manager can't get an audio component"));
		return false;
	}

	Component->SetWorldLocation(NewLocation);
	
	return true;
}

bool UG2IGameSoundManager::ChangeSoundAttachment(int32 SoundId,
	TObjectPtr<USceneComponent> NewAttachmentComponent,
	FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules::KeepRelativeTransform)
{
	if (!ensure(NewAttachmentComponent)) {
		UE_LOG(LogG2I, Warning, TEXT("The sound manager got wrong component to attach"));
		return false;
	}
	TObjectPtr<UAudioComponent> Component = GetAudioById(SoundId);
	if (!ensure(Component)) {
		UE_LOG(LogG2I, Warning, TEXT("The sound manager can't get an audio component"));
		return false;
	}

	Component->AttachToComponent(NewAttachmentComponent, AttachmentRules);

	return true;
}

bool UG2IGameSoundManager::ChangeSoundAttachment(int32 SoundId,
	TObjectPtr<AActor> NewAttachmentActor,
	FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules::KeepRelativeTransform)
{
	if (!ensure(NewAttachmentActor)) {
		UE_LOG(LogG2I, Warning, TEXT("The sound manager got wrong actor to attach"));
		return false;
	}
	TObjectPtr<USceneComponent> RootComponent = NewAttachmentActor->GetRootComponent();
	if (!ensure(RootComponent)) {
		UE_LOG(LogG2I, Warning, TEXT("The sound manager can't get root component from actor to attach"));
		return false;
	}
	TObjectPtr<UAudioComponent> Component = GetAudioById(SoundId);
	if (!ensure(Component)) {
		return false;
	}

	Component->AttachToComponent(RootComponent, AttachmentRules);

	return true;
}


bool UG2IGameSoundManager::RemoveSound(int32 SoundId)
{
	TObjectPtr<UAudioComponent> Component = GetAudioById(SoundId);
	if (!ensure(Component)) {
		return false;
	}

	Component->Stop();
	Component->DestroyComponent();
	ActiveSounds.Remove(SoundId);

	IdStack.Push(SoundId);
	return true;
}

void UG2IGameSoundManager::RemoveAllSounds()
{
	for (const auto& Pair : ActiveSounds)
	{
		if (ensure(Pair.Value))
		{
			Pair.Value->Stop();
			Pair.Value->DestroyComponent();
			IdStack.Push(Pair.Key);
			ActiveSounds.Remove(Pair.Key);
		}
	}
}

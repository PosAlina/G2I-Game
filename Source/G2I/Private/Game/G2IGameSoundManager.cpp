#include "Game/G2IGameSoundManager.h"
#define STARTSIZE 20
#include "G2I.h"
#include "Components/AudioComponent.h"
#include "Components/SceneComponent.h"
#include "Sound/SoundCue.h"

UAudioComponent* UG2IGameSoundManager::GetAudioById(int32 SoundId)
{
	if (SoundId < 0) {
		UE_LOG(LogG2I, Warning, TEXT("Sound manager got wrong SoundId"));
		return TObjectPtr<UAudioComponent>();
	}

	TObjectPtr<UAudioComponent>* FoundAudio = ActiveSounds.Find(SoundId);
	if (FoundAudio) {
		return *FoundAudio;
	}

	UE_LOG(LogG2I, Warning, TEXT("Sound manager: SoundId %d not found"), SoundId);
	return nullptr;
}

UG2IGameSoundManager* UG2IGameSoundManager::Get(UObject* WorldContextObject)
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
	CurrentNumberAvailable = STARTSIZE;
}

void UG2IGameSoundManager::Deinitialize()
{
	StopAllSounds();
	IdStack.Empty();
	ActiveSounds.Empty();

	Super::Deinitialize();
}

int32 UG2IGameSoundManager::AddSound(const FSoundConfig& NewSoundConfig)
{
	if (!ensure(NewSoundConfig.Sound)) {
		UE_LOG(LogG2I, Warning, TEXT("The sound manager didn't get sound"));
		return -1;
	}
	UAudioComponent* AudioComponent = NewObject<UAudioComponent>(GetWorld());
	if (!ensure(AudioComponent)) {
		UE_LOG(LogG2I, Warning, TEXT("The sound manager can't create an audio component"));
		return -1;
	}

	AudioComponent->SetSound(NewSoundConfig.Sound.Get());
	AudioComponent->SetWorldLocation(NewSoundConfig.WorldLocation);

	if (NewSoundConfig.ResolvedAttachComponent) {
		FAttachmentTransformRules Rules(
			NewSoundConfig.AttachmentRules,
			NewSoundConfig.AttachmentRules,
			NewSoundConfig.AttachmentRules,
			false);
		AudioComponent->AttachToComponent(
			NewSoundConfig.ResolvedAttachComponent,
			Rules);
	}
	AudioComponent->SetVolumeMultiplier(NewSoundConfig.VolumeMultiplier);
	AudioComponent->SetPitchMultiplier(NewSoundConfig.PitchMultiplier);
	AudioComponent->RegisterComponent();

	if (IdStack.IsEmpty()) {
		UpdateStackSize();
	}

	int32 NewSoundId = IdStack.Pop();
	ActiveSounds.Add(NewSoundId, AudioComponent);

	return NewSoundId;
}

bool UG2IGameSoundManager::PlaySound(int32 SoundId)
{
	UAudioComponent* Component = GetAudioById(SoundId);
	if (!ensure(Component)) {
		UE_LOG(LogG2I, Warning, TEXT("The sound manager can't get an audio component"));
		return false;
	}

	Component->Play();

	return true;
}

bool UG2IGameSoundManager::StopSound(int32 SoundId)
{
	UAudioComponent* Component = GetAudioById(SoundId);
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
	UAudioComponent* Component = GetAudioById(SoundId);
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
	UAudioComponent* Component = GetAudioById(SoundId);
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
	UAudioComponent* Component = GetAudioById(SoundId);
	if (!ensure(Component)) {
		UE_LOG(LogG2I, Warning, TEXT("The sound manager can't get an audio component"));
		return false;
	}

	Component->SetWorldLocation(NewLocation);
	
	return true;
}

bool UG2IGameSoundManager::ChangeSoundAttachment(int32 SoundId,
	USceneComponent* NewAttachmentComponent,
	EAttachmentRule AttachmentRules)
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

	FAttachmentTransformRules Rules(
		AttachmentRules,
		AttachmentRules,
		AttachmentRules,
		false);

	Component->AttachToComponent(NewAttachmentComponent, Rules);

	return true;
}

bool UG2IGameSoundManager::ChangeSoundAttachment(int32 SoundId,
	AActor* NewAttachmentActor,
	EAttachmentRule AttachmentRules)
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
	
	FAttachmentTransformRules Rules(
		AttachmentRules,
		AttachmentRules,
		AttachmentRules,
		false);

	Component->AttachToComponent(RootComponent, Rules);

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
	for (auto Pair = ActiveSounds.CreateIterator(); Pair; ++Pair)
	{
		if (ensure(Pair.Value()))
		{
			Pair.Value()->Stop();
			Pair.Value()->DestroyComponent();
		}
	}
	ActiveSounds.Empty();
	IdStack.Empty();
	CurrentNumberAvailable = STARTSIZE;
	UpdateStackSize();
}

void UG2IGameSoundManager::UpdateStackSize() {
	for (int32 i = CurrentNumberAvailable; i < CurrentNumberAvailable * 2; ++i) {
		IdStack.Push(i);
	}
	CurrentNumberAvailable *= 2;
}
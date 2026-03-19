#include "Components/G2ISoundComponent.h"

#include "G2I.h"

void UG2ISoundComponent::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* World = GetWorld();
	if (!ensure(World)) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the World in %s"), *GetName());
		return;
	}
	SoundManager = World->GetSubsystem<UG2IGameSoundManager>();
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
	}
}


int32 UG2ISoundComponent::AddSound(const FSoundConfig* NewSoundConfig)
{
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
		return -1;
	}
	if (!NewSoundConfig) {
		UE_LOG(LogG2I, Warning, TEXT("Got empty config in %s"), *GetName());
		return -1;
	}

	FSoundConfig ConfigToSend = *NewSoundConfig;

	ConfigToSend.ResolvedAttachComponent = Cast<USceneComponent>(ConfigToSend.AttachToComponent.GetComponent(GetOwner()));

	return SoundManager->AddSound(&ConfigToSend);
}

bool UG2ISoundComponent::PlaySound(int32 SoundId)
{
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
		return false;
	}
	return SoundManager->PlaySound(SoundId);
}

bool UG2ISoundComponent::StopSound(int32 SoundId)
{
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
		return false;
	}
	return SoundManager->StopSound(SoundId);
}

void UG2ISoundComponent::StopAllSounds()
{
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
		return;
	}
	SoundManager->StopAllSounds();
}

bool UG2ISoundComponent::ChangeSoundVolume(int32 SoundId, float NewVolume)
{
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
		return false;
	}
	return SoundManager->ChangeSoundVolume(SoundId, NewVolume);
}

bool UG2ISoundComponent::ChangeSoundPitch(int32 SoundId, float NewPitch)
{
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
		return false;
	}
	return SoundManager->ChangeSoundPitch(SoundId, NewPitch);
}

bool UG2ISoundComponent::ChangeSoundLocation(int32 SoundId, FVector NewLocation)
{
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
		return false;
	}
	return SoundManager->ChangeSoundLocation(SoundId, NewLocation);
}

bool UG2ISoundComponent::ChangeSoundAttachment(int32 SoundId,
	USceneComponent* NewAttachmentComponent,
	FAttachmentTransformRules AttachmentRules)
{
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
		return false;
	}
	return SoundManager->ChangeSoundAttachment(SoundId, NewAttachmentComponent, AttachmentRules);
}

bool UG2ISoundComponent::ChangeSoundAttachment(int32 SoundId,
	AActor* NewAttachmentActor,
	FAttachmentTransformRules AttachmentRules)
{
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
		return false;
	}
	return SoundManager->ChangeSoundAttachment(SoundId, NewAttachmentActor, AttachmentRules);
}


bool UG2ISoundComponent::RemoveSound(int32 SoundId)
{
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
		return false;
	}
	return SoundManager->RemoveSound(SoundId);
}

void UG2ISoundComponent::RemoveAllSounds()
{
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
		return;
	}
	SoundManager->RemoveAllSounds();
}


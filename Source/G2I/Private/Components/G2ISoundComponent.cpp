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


int32 UG2ISoundComponent::AddSound(const FSoundConfig& NewSoundConfig)
{
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
		return -1;
	}

	FSoundConfig ConfigToSend = NewSoundConfig;
	AActor* Owner = GetOwner();
	if (!ensure(Owner)) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the owner of component in %s"), *GetName());
		return -1;
	}

	

	ConfigToSend.ResolvedAttachComponent = Cast<USceneComponent>(ConfigToSend.AttachToComponent.GetComponent(Owner));
	
	if (!ConfigToSend.ResolvedAttachComponent && !ConfigToSend.bIs2D)
	{
		ConfigToSend.ResolvedAttachComponent = this;
	}

	return SoundManager->AddSound(ConfigToSend);
}

bool UG2ISoundComponent::PlaySound(int32 SoundId)
{
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
		return false;
	}
	return SoundManager->PlaySound(SoundId);
}

bool UG2ISoundComponent::StopSound(int32 SoundId, float FadeOutTime)
{
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
		return false;
	}
	return SoundManager->StopSound(SoundId, FadeOutTime);
}

void UG2ISoundComponent::StopAllSounds()
{
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
		return;
	}
	SoundManager->StopAllSounds();
}

bool UG2ISoundComponent::SetSoundVolume(int32 SoundId, float NewVolume)
{
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
		return false;
	}
	return SoundManager->SetSoundVolume(SoundId, NewVolume);
}

bool UG2ISoundComponent::SetSoundPitch(int32 SoundId, float NewPitch)
{
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
		return false;
	}
	return SoundManager->SetSoundPitch(SoundId, NewPitch);
}

bool UG2ISoundComponent::SetSoundLocation(int32 SoundId, FVector NewLocation)
{
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
		return false;
	}
	return SoundManager->SetSoundLocation(SoundId, NewLocation);
}

bool UG2ISoundComponent::SetSoundAttachment(int32 SoundId,
	USceneComponent* NewAttachmentComponent,
	EAttachmentRule AttachmentRules)
{
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
		return false;
	}

	return SoundManager->SetSoundAttachment(SoundId, NewAttachmentComponent, AttachmentRules);
}

bool UG2ISoundComponent::SetSoundAttachment(int32 SoundId,
	AActor* NewAttachmentActor,
	EAttachmentRule AttachmentRules)
{
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
		return false;
	}

	return SoundManager->SetSoundAttachment(SoundId, NewAttachmentActor, AttachmentRules);
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

bool UG2ISoundComponent::SetSoundLooping(int32 SoundId, bool bNewIsLooping)
{
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
		return false;
	}
	return SoundManager->SetSoundLooping(SoundId, bNewIsLooping);
}

bool UG2ISoundComponent::SetSoundAutoDestroy(int32 SoundId, bool bNewAutoDestroy)
{
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
		return false;
	}
	return SoundManager->SetSoundAutoDestroy(SoundId, bNewAutoDestroy);
}

float UG2ISoundComponent::GetSoundVolume(int32 SoundId)
{
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
		return 0.0f;
	}
	return SoundManager->GetSoundVolume(SoundId);
}

float UG2ISoundComponent::GetSoundPitch(int32 SoundId)
{
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
		return 1.0f;
	}
	return SoundManager->GetSoundPitch(SoundId);
}

FVector UG2ISoundComponent::GetSoundLocation(int32 SoundId)
{
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
		return FVector::ZeroVector;
	}
	return SoundManager->GetSoundLocation(SoundId);
}

bool UG2ISoundComponent::GetSoundLooping(int32 SoundId)
{
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
		return false;
	}
	return SoundManager->GetSoundLooping(SoundId);
}

bool UG2ISoundComponent::GetSoundAutoDestroy(int32 SoundId)
{
	if (!SoundManager.IsValid()) {
		UE_LOG(LogG2I, Error, TEXT("Couldn't get the Sound Manager in %s"), *GetName());
		return false;
	}
	return SoundManager->GetSoundAutoDestroy(SoundId);
}
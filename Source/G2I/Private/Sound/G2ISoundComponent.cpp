#include "Sound/G2ISoundComponent.h"
#include "G2I.h"

void UG2ISoundComponent::BeginPlay()
{
	Super::BeginPlay();

	const UWorld* World = GetWorld();
	if (!ensure(World)) {
		UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Couldn't get the World"), *GetName(), *FString(__FUNCTION__));
		return;
	}
	SoundManager = UG2IGameSoundManager::Get(World);
	if (!ensure(SoundManager)) {
		UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Couldn't get the Sound Manager"), *GetName(), *FString(__FUNCTION__));
	}
}


int32 UG2ISoundComponent::AddSound(const FSoundConfig& NewSoundConfig)
{
	if (!SoundManager)
	{
		SoundManager = UG2IGameSoundManager::Get(this);
		if (!ensure(SoundManager))
		{
			UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Couldn't get the Sound Manager"), *GetName(), *FString(__FUNCTION__));
			return -1;
		}
	}

	FSoundConfig ConfigToSend = NewSoundConfig;

	AActor* TargetActor = ConfigToSend.AttachToComponent.OtherActor.Get();

	if (!TargetActor)
	{
		ConfigToSend.ResolvedAttachComponent = nullptr;
	}
	else
	{
		USceneComponent* TargetComp = Cast<USceneComponent>(ConfigToSend.AttachToComponent.GetComponent(TargetActor));

		if (TargetComp)
		{
			ConfigToSend.ResolvedAttachComponent = TargetComp;
		}
		else
		{
			ConfigToSend.ResolvedAttachComponent = TargetActor->GetRootComponent();

			if (!ConfigToSend.ResolvedAttachComponent)
			{
				UE_LOG(LogG2I, Warning, TEXT("[%s]: TargetActor %s has no RootComponent! Falling back to Location."), *GetName(), *TargetActor->GetName());

				ConfigToSend.ResolvedAttachComponent = nullptr;
				ConfigToSend.WorldLocation = TargetActor->GetActorLocation();
			}
		}
	}

	return SoundManager->AddSound(ConfigToSend);
}

bool UG2ISoundComponent::PlaySound(const int32 SoundId)
{
	if (!SoundManager)
	{
		SoundManager = UG2IGameSoundManager::Get(this);
		if (!ensure(SoundManager)) {
			UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Couldn't get the Sound Manager"), *GetName(), *FString(__FUNCTION__));
			return false;
		}
	}
	if (SoundId == -1) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: ID = -1"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	return SoundManager->PlaySound(SoundId);
}

bool UG2ISoundComponent::PlaySoundSafe(UG2ISoundComponent* SoundComp, const int32 SoundId)
{
	if (!ensure(SoundComp))
	{
		UE_LOG(LogG2I, Error, TEXT("[UG2ISoundComponent::PlaySoundSafe]: SoundComp is invalid!"));
		return false;
	}

	return SoundComp->PlaySound(SoundId);
}

int32 UG2ISoundComponent::AddSoundSafe(UG2ISoundComponent* SoundComp, const FSoundConfig& NewSoundConfig)
{
	if (!ensure(SoundComp))
	{
		UE_LOG(LogG2I, Error, TEXT("[UG2ISoundComponent::AddSoundSafe]: SoundComp is invalid!"));
		return -1;
	}

	return SoundComp->AddSound(NewSoundConfig);
}

bool UG2ISoundComponent::StopSoundSafe(UG2ISoundComponent* SoundComp, const int32 SoundId) {
	if (!ensure(SoundComp))
	{
		UE_LOG(LogG2I, Error, TEXT("[UG2ISoundComponent::PlaySoundSafe]: SoundComp is invalid!"));
		return false;
	}

	return SoundComp->StopSound(SoundId);
}

bool UG2ISoundComponent::StopSound(const int32 SoundId, const float FadeOutTime)
{
	if (!SoundManager)
	{
		SoundManager = UG2IGameSoundManager::Get(this);
		if (!ensure(SoundManager)) {
			UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Couldn't get the Sound Manager"), *GetName(), *FString(__FUNCTION__));
			return false;
		}
	}
	if (SoundId == -1) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: ID = -1"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	return SoundManager->StopSound(SoundId, FadeOutTime);
}

void UG2ISoundComponent::StopAllSounds() const
{
	if (!ensure(SoundManager)) {
		UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Couldn't get the Sound Manager"), *GetName(), *FString(__FUNCTION__));
		return;
	}
	SoundManager->StopAllSounds();
}

bool UG2ISoundComponent::SetSoundVolume(const int32 SoundId, const float NewVolume) const
{
	if (!ensure(SoundManager)) {
		UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Couldn't get the Sound Manager"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	if (SoundId == -1) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: ID = -1"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	return SoundManager->SetSoundVolume(SoundId, NewVolume);
}

bool UG2ISoundComponent::SetSoundPitch(const int32 SoundId, const float NewPitch) const
{
	if (!ensure(SoundManager)) {
		UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Couldn't get the Sound Manager"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	if (SoundId == -1) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: ID = -1"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	return SoundManager->SetSoundPitch(SoundId, NewPitch);
}

bool UG2ISoundComponent::SetSoundLocation(const int32 SoundId, const FVector& NewLocation) const
{
	if (!ensure(SoundManager)) {
		UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Couldn't get the Sound Manager"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	if (SoundId == -1) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: ID = -1"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	return SoundManager->SetSoundLocation(SoundId, NewLocation);
}

bool UG2ISoundComponent::SetSoundAttachmentComponent(const int32 SoundId,
	USceneComponent* NewAttachmentComponent,
	const EAttachmentRule AttachmentRules) const
{
	if (!ensure(SoundManager)) {
		UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Couldn't get the Sound Manager"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	if (SoundId == -1) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: ID = -1"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	return SoundManager->SetSoundAttachment(SoundId, NewAttachmentComponent, AttachmentRules);
}

bool UG2ISoundComponent::SetSoundAttachmentActor(const int32 SoundId,
	AActor* NewAttachmentActor,
	const EAttachmentRule AttachmentRules) const
{
	if (!ensure(SoundManager)) {
		UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Couldn't get the Sound Manager"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	if (SoundId == -1) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: ID = -1"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	return SoundManager->SetSoundAttachment(SoundId, NewAttachmentActor, AttachmentRules);
}

bool UG2ISoundComponent::RemoveSound(const int32 SoundId) const
{
	if (!ensure(SoundManager)) {
		UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Couldn't get the Sound Manager"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	if (SoundId == -1) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: ID = -1"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	return SoundManager->RemoveSound(SoundId);
}

bool UG2ISoundComponent::SetSoundAutoDestroy(const int32 SoundId, const bool bNewAutoDestroy) const
{
	if (!ensure(SoundManager)) {
		UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Couldn't get the Sound Manager"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	if (SoundId == -1) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: ID = -1"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	return SoundManager->SetSoundAutoDestroy(SoundId, bNewAutoDestroy);
}

bool UG2ISoundComponent::SetSoundPlayingOneTime(const int32 SoundId, const bool bNewIsPlayingOneTime) const
{
	if (!ensure(SoundManager)) {
		UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Couldn't get the Sound Manager"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	if (SoundId == -1) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: ID = -1"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	return SoundManager->SetSoundPlayingOneTime(SoundId, bNewIsPlayingOneTime);
}

bool UG2ISoundComponent::SetSoundPlayingFromStart(const int32 SoundId, const bool bNewIsPlayingFromStart) const
{
	if (!ensure(SoundManager)) {
		UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Couldn't get the Sound Manager"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	if (SoundId == -1) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: ID = -1"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	return SoundManager->SetSoundPlayingFromStart(SoundId, bNewIsPlayingFromStart);
}



float UG2ISoundComponent::GetSoundVolume(const int32 SoundId) const
{
	if (!ensure(SoundManager)) {
		UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Couldn't get the Sound Manager"), *GetName(), *FString(__FUNCTION__));
		return 0.0f;
	}
	if (SoundId == -1) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: ID = -1"), *GetName(), *FString(__FUNCTION__));
		return 0.0f;
	}
	return SoundManager->GetSoundVolume(SoundId);
}

float UG2ISoundComponent::GetSoundPitch(const int32 SoundId) const
{
	if (!ensure(SoundManager)) {
		UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Couldn't get the Sound Manager"), *GetName(), *FString(__FUNCTION__));
		return 1.0f;
	}
	if (SoundId == -1) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: ID = -1"), *GetName(), *FString(__FUNCTION__));
		return 1.0f; 
	}
	return SoundManager->GetSoundPitch(SoundId);
}

FVector UG2ISoundComponent::GetSoundLocation(const int32 SoundId) const
{
	if (!ensure(SoundManager)) {
		UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Couldn't get the Sound Manager"), *GetName(), *FString(__FUNCTION__));
		return FVector::ZeroVector;
	}
	if (SoundId == -1) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: ID = -1"), *GetName(), *FString(__FUNCTION__));
		return FVector::ZeroVector;
	}
	return SoundManager->GetSoundLocation(SoundId);
}

bool UG2ISoundComponent::GetSoundAutoDestroy(const int32 SoundId) const
{
	if (!ensure(SoundManager)) {
		UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Couldn't get the Sound Manager"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	if (SoundId == -1) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: ID = -1"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	return SoundManager->GetSoundAutoDestroy(SoundId);
}

bool UG2ISoundComponent::IsSoundPlaying(const int32 SoundId) const
{
	if (!ensure(SoundManager)) {
		UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Couldn't get the Sound Manager"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	if (SoundId == -1) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: ID = -1"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	return SoundManager->IsSoundPlaying(SoundId);
}

bool UG2ISoundComponent::GetSoundPlayingFromStart(const int32 SoundId) const
{
	if (!ensure(SoundManager)) {
		UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Couldn't get the Sound Manager"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	if (SoundId == -1) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: ID = -1"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	return SoundManager->GetSoundPlayingFromStart(SoundId);
}

bool UG2ISoundComponent::GetSoundPlayingOneTime(const int32 SoundId) const
{
	if (!ensure(SoundManager)) {
		UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Couldn't get the Sound Manager"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	if (SoundId == -1) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: ID = -1"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	return SoundManager->GetSoundPlayingOneTime(SoundId);
}
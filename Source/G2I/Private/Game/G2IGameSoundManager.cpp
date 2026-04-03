#include "Game/G2IGameSoundManager.h"
#define STARTSIZE 20
#include "G2I.h"
#include "Components/AudioComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

static const FName OneTimeTag("PlayingOneTime");

UAudioComponent* UG2IGameSoundManager::GetAudioById(int32 SoundId)
{
	if (SoundId < 0) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Wrong SoundId: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
		return nullptr;
	}

	TObjectPtr<UAudioComponent>* FoundAudio = ActiveSounds.Find(SoundId);
	if (FoundAudio) {
		return *FoundAudio;
	}

	UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: SoundId %d not found"), *GetName(), *FString(__FUNCTION__), SoundId);
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
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: The sound manager didn't get sound"), *GetName(), *FString(__FUNCTION__));
		return -1;
	}
	UAudioComponent* AudioComponent = NewObject<UAudioComponent>(GetWorld()->GetWorldSettings());
	if (!ensure(AudioComponent)) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: The sound manager can't create an audio component"), *GetName(), *FString(__FUNCTION__));
		return -1;
	}

	AudioComponent->SetSound(NewSoundConfig.Sound.Get());
	AudioComponent->SetWorldLocation(NewSoundConfig.WorldLocation);

	if (!NewSoundConfig.bIs2D)
	{
		AudioComponent->SetWorldLocation(NewSoundConfig.WorldLocation);

		if (NewSoundConfig.ResolvedAttachComponent) {
			FAttachmentTransformRules Rules(
				NewSoundConfig.AttachmentRules,
				NewSoundConfig.AttachmentRules,
				NewSoundConfig.AttachmentRules,
				false);
			AudioComponent->AttachToComponent(NewSoundConfig.ResolvedAttachComponent, Rules);
		}
	}
	else
	{
		AudioComponent->bAllowSpatialization = false;
	}
	AudioComponent->SetVolumeMultiplier(NewSoundConfig.VolumeMultiplier);
	AudioComponent->SetPitchMultiplier(NewSoundConfig.PitchMultiplier);

	bool bAssetIsLooping = NewSoundConfig.Sound->IsLooping();

	AudioComponent->bAutoDestroy = bAssetIsLooping ? false : NewSoundConfig.bAutoDestroy;

	AudioComponent->RegisterComponent();

	if (IdStack.IsEmpty()) {
		UpdateStackSize();
	}

	int32 NewSoundId = IdStack.Pop();
	ActiveSounds.Add(NewSoundId, AudioComponent);
	AudioComponent->OnAudioFinishedNative.AddUObject(this, &UG2IGameSoundManager::OnSoundFinished, NewSoundId);

	return NewSoundId;
}

bool UG2IGameSoundManager::PlaySound(int32 SoundId, float FadeInTime)
{
	UAudioComponent* Component = GetAudioById(SoundId);
	if (!ensure(Component)) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: The sound manager can't get an audio component for ID: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
		return false;
	}

	Component->FadeIn(FadeInTime, Component->VolumeMultiplier);

	return true;
}

bool UG2IGameSoundManager::StopSound(int32 SoundId, float FadeOutTime)
{
	UAudioComponent* Component = GetAudioById(SoundId);
	if (!ensure(Component)) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: The sound manager can't get an audio component for ID: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
		return false;
	}

	Component->FadeOut(FadeOutTime, 0.0f);

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

bool UG2IGameSoundManager::SetSoundVolume(int32 SoundId, float NewVolume)
{
	UAudioComponent* Component = GetAudioById(SoundId);
	if (!ensure(Component)) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: The sound manager can't get an audio component for ID: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
		return false;
	}

	NewVolume = FMath::Clamp(NewVolume, 0.0f, 1.0f);
	Component->SetVolumeMultiplier(NewVolume);

	return true;
}

bool UG2IGameSoundManager::SetSoundPitch(int32 SoundId, float NewPitch)
{
	UAudioComponent* Component = GetAudioById(SoundId);
	if (!ensure(Component)) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: The sound manager can't get an audio component for ID: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
		return false;
	}

	NewPitch = FMath::Clamp(NewPitch, 0.0f, 2.0f);
	Component->SetPitchMultiplier(NewPitch);

	return true;
}

bool UG2IGameSoundManager::SetSoundLocation(int32 SoundId, FVector NewLocation)
{
	UAudioComponent* Component = GetAudioById(SoundId);
	if (!ensure(Component)) {
		return false;
	}

	Component->SetWorldLocation(NewLocation);

	return true;
}

bool UG2IGameSoundManager::SetSoundAttachment(int32 SoundId,
	USceneComponent* NewAttachmentComponent,
	EAttachmentRule AttachmentRules)
{
	if (!ensure(NewAttachmentComponent)) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: The sound manager got wrong component to attach"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	TObjectPtr<UAudioComponent> Component = GetAudioById(SoundId);
	if (!ensure(Component)) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: The sound manager can't get an audio component for ID: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
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

bool UG2IGameSoundManager::SetSoundAttachment(int32 SoundId,
	AActor* NewAttachmentActor,
	EAttachmentRule AttachmentRules)
{
	if (!ensure(NewAttachmentActor)) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: The sound manager got wrong actor to attach"), *GetName(), *FString(__FUNCTION__));
		return false;
	}
	TObjectPtr<USceneComponent> RootComponent = NewAttachmentActor->GetRootComponent();
	if (!ensure(RootComponent)) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: The sound manager can't get root component from actor to attach"), *GetName(), *FString(__FUNCTION__));
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
	if (!Component) {
		return false;
	}

	Component->OnAudioFinishedNative.RemoveAll(this);

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
			Pair.Value()->OnAudioFinishedNative.RemoveAll(this);

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

void UG2IGameSoundManager::InitGlobalAudio(USoundMix* _MainMix, TMap<EG2IASoundType, USoundClass*> _SoundClasses)
{
	MainSoundMix = _MainMix;
	SoundClasses = _SoundClasses;
	UWorld* World = GetWorld();
	if (MainSoundMix && World)
	{
		UGameplayStatics::PushSoundMixModifier(World, MainSoundMix);
	}
	else {
		UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Can't get the World or MainSoundMix"), *GetName(), *FString(__FUNCTION__));
	}
}

void UG2IGameSoundManager::SetGlobalVolume(EG2IASoundType SoundType, float NewVolume)
{
	if (!MainSoundMix)
	{
		UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Can't get the MainSoundMix"), *GetName(), *FString(__FUNCTION__));
		return;
	}

	if (!SoundClasses.Contains(SoundType))
	{
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: SoundClasses map doesn't contain SoundType (ID: %d)"), *GetName(), *FString(__FUNCTION__), static_cast<int32>(SoundType));
		return;
	}

	USoundClass* TargetClass = SoundClasses[SoundType];
	if (!TargetClass)
	{
		UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Can't get Sound class for the SoundType (ID: %d)"), *GetName(), *FString(__FUNCTION__), static_cast<int32>(SoundType));
		return;
	}

	float ClampedVolume = FMath::Clamp(NewVolume, 0.0f, 1.0f);

	switch (SoundType)
	{
		case EG2IASoundType::DefaultSound:
		{
			GeneralSoundMultiplier = ClampedVolume;
			break;
		}
		case EG2IASoundType::EffectSound:
		{
			EffectsSoundMultiplier = ClampedVolume;
			break;
		}
		case EG2IASoundType::MusicSound:
		{
			MusicSoundMultiplier = ClampedVolume;
			break;
		}
		case EG2IASoundType::SpeechSound:
		{
			SpeechSoundMultiplier = ClampedVolume;
			break;
		}
	}

	if (UWorld* World = GetWorld()) {
		UGameplayStatics::SetSoundMixClassOverride(
			World,
			MainSoundMix,
			TargetClass,
			ClampedVolume,
			1.0f,
			0.0f,
			true
		);
	}
	else {
		UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Can't get the World"), *GetName(), *FString(__FUNCTION__));
	}
}

float UG2IGameSoundManager::GetGlobalVolume(EG2IASoundType SoundType) const {
	switch (SoundType) {
		case EG2IASoundType::DefaultSound:
		{
			return GeneralSoundMultiplier;
		}
		case EG2IASoundType::EffectSound:
		{
			return EffectsSoundMultiplier;
		}
		case EG2IASoundType::MusicSound:
		{
			return MusicSoundMultiplier;
		}
		case EG2IASoundType::SpeechSound:
		{
			return SpeechSoundMultiplier;
		}
		default: {
			return 0.0f;
		}
	}
}

void UG2IGameSoundManager::OnSoundFinished(UAudioComponent* AudioComponent, int32 SoundId)
{
	if (!AudioComponent) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Can't get AudioComponent when sound with ID %d finished"), *GetName(), *FString(__FUNCTION__), SoundId);
		ActiveSounds.Remove(SoundId);
		IdStack.Push(SoundId);
		return;
	}

	if (AudioComponent->bAutoDestroy)
	{
		ActiveSounds.Remove(SoundId);
		IdStack.Push(SoundId);
	}
}
float UG2IGameSoundManager::GetSoundVolume(int32 SoundId)
{
	if (UAudioComponent* Component = GetAudioById(SoundId))
	{
		return Component->VolumeMultiplier;
	}

	UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Invalid SoundId: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
	return 0.0f;
}

float UG2IGameSoundManager::GetSoundPitch(int32 SoundId)
{
	if (UAudioComponent* Component = GetAudioById(SoundId))
	{
		return Component->PitchMultiplier;
	}

	UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Invalid SoundId: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
	return 1.0f;
}

FVector UG2IGameSoundManager::GetSoundLocation(int32 SoundId)
{
	if (UAudioComponent* Component = GetAudioById(SoundId))
	{
		return Component->GetComponentLocation();
	}

	UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Invalid SoundId: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
	return FVector::ZeroVector;
}

USceneComponent* UG2IGameSoundManager::GetSoundAttachment(int32 SoundId)
{
	if (UAudioComponent* Component = GetAudioById(SoundId))
	{
		return Component->GetAttachParent();
	}

	UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Invalid SoundId: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
	return nullptr;
}

bool UG2IGameSoundManager::SetSoundAutoDestroy(int32 SoundId, bool bNewAutoDestroy)
{
	UAudioComponent* Component = GetAudioById(SoundId);
	if (!Component)
	{
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Invalid SoundId: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
		return false;
	}
	Component->bAutoDestroy = bNewAutoDestroy;

	return true;
}

bool UG2IGameSoundManager::GetSoundAutoDestroy(int32 SoundId)
{
	if (UAudioComponent* Component = GetAudioById(SoundId))
	{
		return Component->bAutoDestroy;
	}

	UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Invalid SoundId: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
	return false;
}

bool UG2IGameSoundManager::GetSoundPlayingOneTime(int32 SoundId)
{
	if (UAudioComponent* Component = GetAudioById(SoundId))
	{
		return Component->ComponentHasTag(OneTimeTag);
	}

	UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Invalid SoundId: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
	return false;
}

bool UG2IGameSoundManager::SetSoundPlayingOneTime(int32 SoundId, bool bNewIsPlayingOneTime)
{
	UAudioComponent* Component = GetAudioById(SoundId);
	if (!Component)
	{
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Invalid SoundId: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
		return false;
	}

	if (bNewIsPlayingOneTime)
	{
		Component->ComponentTags.AddUnique(OneTimeTag);
		Component->bAutoDestroy = true;
	}
	else
	{
		Component->ComponentTags.Remove(OneTimeTag);
	}

	return true;
}
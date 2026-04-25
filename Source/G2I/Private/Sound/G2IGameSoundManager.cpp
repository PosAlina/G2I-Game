#include "Sound/G2IGameSoundManager.h"

#include "G2I.h"
#include "Components/AudioComponent.h"
#include "Components/SceneComponent.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
#include "Kismet/GameplayStatics.h"
#include "G2IGameInstance.h"

static const FName OneTimeTag("PlayingOneTime");
constexpr int32 STARTSOUNDSTACKSIZE = 20;

TObjectPtr<UAudioComponent> UG2IGameSoundManager::GetAudioById(const int32 SoundId) const
{
	if (SoundId < 0) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Wrong SoundId: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
		return nullptr;
	}

	const TObjectPtr<UAudioComponent>* FoundAudio = ActiveSounds.Find(SoundId);
	if (FoundAudio) {
		return *FoundAudio;
	}

	UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: SoundId %d not found"), *GetName(), *FString(__FUNCTION__), SoundId);
	return nullptr;
}

UG2IGameSoundManager* UG2IGameSoundManager::Get(const UObject* WorldContextObject)
{
	if (!GEngine)
	{
		return nullptr;
	}

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull))
	{
		if (!World->IsGameWorld())
		{
			return nullptr;
		}

		if (const UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance->GetSubsystem<UG2IGameSoundManager>();
		}
		else
		{
			UE_LOG(LogG2I, Error, TEXT("SoundManager Get: GameInstance is NULL in Game World for %s!"), *WorldContextObject->GetName());
		}
	}

	return nullptr;
}

void UG2IGameSoundManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	for (int32 i = 0; i < STARTSOUNDSTACKSIZE; ++i)
	{
		IdStack.Push(i);
	}
	CurrentNumberAvailable = STARTSOUNDSTACKSIZE;

	GameInstance = Cast<UG2IGameInstance>(GetGameInstance());
	if (!ensure(GameInstance))
	{
		UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Couldn't find %s"), *GetName(), *FString(__FUNCTION__), *UG2IGameInstance::StaticClass()->GetName());
		return;
	}

	StartGameDelegateHandle = GameInstance->OnStartLevelInitDelegate.AddUObject(this, &ThisClass::InitializeInStartGame);
}

void UG2IGameSoundManager::InitializeInStartGame()
{
	if (!ensure(GameInstance))
	{
		UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Couldn't find %s"), *GetName(), *FString(__FUNCTION__), *UG2IGameInstance::StaticClass()->GetName());
		return;
	}
	GameInstance->OnStartLevelInitDelegate.Remove(StartGameDelegateHandle);

	InitializeInStartLevel();

	GameInstance->OnCloseLevelDelegate.AddUObject(this, &ThisClass::CloseLevelSound);
	GameInstance->OnStartLevelInitDelegate.AddUObject(this, &ThisClass::InitializeInStartLevel);
}

void UG2IGameSoundManager::InitializeInStartLevel()
{
	UWorld* NewWorld = GetWorld();

	if (MainSoundMix && NewWorld)
	{
		UGameplayStatics::PushSoundMixModifier(NewWorld, MainSoundMix);

		for (const auto& Iterator : GlobalSoundMultipliers)
		{
			if (USoundClass* TargetClass = SoundClasses.FindRef(Iterator.Key))
			{
				UGameplayStatics::SetSoundMixClassOverride(
					NewWorld,
					MainSoundMix,
					TargetClass,
					Iterator.Value,
					1.0f,
					0.0f,
					true
				);
			}
		}
	}
}

void UG2IGameSoundManager::CloseLevelSound()
{
	ActiveSounds.Empty();
	IdStack.Empty();
	CurrentNumberAvailable = STARTSOUNDSTACKSIZE;

	for (int32 i = 0; i < CurrentNumberAvailable; ++i) {
		IdStack.Push(i);
	}
}

void UG2IGameSoundManager::Deinitialize()
{
	StopAllSounds();
	ActiveSounds.Empty();
	IdStack.Empty();
	
	if (GameInstance)
	{
		GameInstance->OnStartLevelInitDelegate.RemoveAll(this);
		GameInstance->OnCloseLevelDelegate.RemoveAll(this);
	}

	Super::Deinitialize();
}

int32 UG2IGameSoundManager::AddSound(const FSoundConfig& NewSoundConfig)
{
	if (!ensure(NewSoundConfig.Sound)) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: The sound manager didn't get sound"), *GetName(), *FString(__FUNCTION__));
		return -1;
	}

	const UWorld* World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: World is null"), *GetName(), *FString(__FUNCTION__));
		return -1;
	}

	AWorldSettings* WorldSettings = World->GetWorldSettings();
	if (!ensure(WorldSettings))
	{
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Can't get WorldSettings"), *GetName(), *FString(__FUNCTION__));
		return -1;
	}

	UAudioComponent* AudioComponent = NewObject<UAudioComponent>(WorldSettings);
	if (!ensure(AudioComponent)) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: The sound manager can't create an audio component"), *GetName(), *FString(__FUNCTION__));
		return -1;
	}

	AudioComponent->RegisterComponent();

	AudioComponent->SetSound(NewSoundConfig.Sound.Get());

	if (!NewSoundConfig.bIs2D)
	{
		AudioComponent->SetWorldLocation(NewSoundConfig.WorldLocation);

		if (NewSoundConfig.ResolvedAttachComponent) {
			const FAttachmentTransformRules Rules(
				NewSoundConfig.AttachmentRules,
				NewSoundConfig.AttachmentRules,
				NewSoundConfig.AttachmentRules,
				false);
			if (!NewSoundConfig.ResolvedAttachComponent->IsRegistered()) {
				NewSoundConfig.ResolvedAttachComponent->RegisterComponent();
			}
			AudioComponent->AttachToComponent(NewSoundConfig.ResolvedAttachComponent, Rules);
		}
	}
	else
	{
		AudioComponent->bAllowSpatialization = false;
	}

	AudioComponent->SetVolumeMultiplier(NewSoundConfig.VolumeMultiplier);
	AudioComponent->SetPitchMultiplier(NewSoundConfig.PitchMultiplier);

	const bool bAssetIsLooping = NewSoundConfig.Sound->IsLooping();

	AudioComponent->bAutoDestroy = bAssetIsLooping ? false : NewSoundConfig.bAutoDestroy;

	if (IdStack.IsEmpty()) {
		UpdateStackSize();
	}

	const int32 NewSoundId = IdStack.Pop();
	ActiveSounds.Add(NewSoundId, AudioComponent);
	AudioComponent->OnAudioFinishedNative.AddUObject(this, &UG2IGameSoundManager::OnSoundFinished, NewSoundId);

	return NewSoundId;
}

bool UG2IGameSoundManager::PlaySound(const int32 SoundId, const float FadeInTime)
{
	UAudioComponent* Component = GetAudioById(SoundId);
	if (!ensure(Component)) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: The sound manager can't get an audio component for ID: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
		return false;
	}

	Component->FadeIn(FadeInTime, Component->VolumeMultiplier);

	return true;
}

bool UG2IGameSoundManager::StopSound(const int32 SoundId, const float FadeOutTime)
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

bool UG2IGameSoundManager::SetSoundVolume(const int32 SoundId, float NewVolume)
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

bool UG2IGameSoundManager::SetSoundPitch(const int32 SoundId, float NewPitch)
{
	UAudioComponent* Component = GetAudioById(SoundId);
	if (!ensure(Component)) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: The sound manager can't get an audio component for ID: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
		return false;
	}

	if (NewPitch <= 0.0f) {
		NewPitch = 0.0f;
	}
	Component->SetPitchMultiplier(NewPitch);

	return true;
}

bool UG2IGameSoundManager::SetSoundLocation(const int32 SoundId, const FVector& NewLocation)
{
	UAudioComponent* Component = GetAudioById(SoundId);
	if (!ensure(Component)) {
		return false;
	}

	Component->SetWorldLocation(NewLocation);

	return true;
}

bool UG2IGameSoundManager::SetSoundAttachment(const int32 SoundId,
	USceneComponent* NewAttachmentComponent,
	const EAttachmentRule AttachmentRules)
{
	if (!ensure(NewAttachmentComponent)) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: The sound manager got wrong component to attach"), *GetName(), *FString(__FUNCTION__));
		return false;
	}

	const TObjectPtr<UAudioComponent> Component = GetAudioById(SoundId);
	if (!ensure(Component)) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: The sound manager can't get an audio component for ID: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
		return false;
	}

	const FAttachmentTransformRules Rules(
		AttachmentRules,
		AttachmentRules,
		AttachmentRules,
		false);

	if (!NewAttachmentComponent->IsRegistered()) {
		NewAttachmentComponent->RegisterComponent();
	}
	Component->AttachToComponent(NewAttachmentComponent, Rules);

	return true;
}

bool UG2IGameSoundManager::SetSoundAttachment(const int32 SoundId,
	AActor* NewAttachmentActor,
	const EAttachmentRule AttachmentRules)
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

	const TObjectPtr<UAudioComponent> Component = GetAudioById(SoundId);
	if (!ensure(Component)) {
		return false;
	}

	const FAttachmentTransformRules Rules(
		AttachmentRules,
		AttachmentRules,
		AttachmentRules,
		false);
	if (!RootComponent->IsRegistered()) {
		RootComponent->RegisterComponent();
	}
	Component->AttachToComponent(RootComponent, Rules);

	return true;
}


bool UG2IGameSoundManager::RemoveSound(const int32 SoundId)
{
	TObjectPtr<UAudioComponent> Component = GetAudioById(SoundId);
	if (!Component) {
		return false;
	}

	Component->OnAudioFinishedNative.RemoveAll(this);

	Component->Stop();
	ActiveSounds.Remove(SoundId);

	IdStack.Push(SoundId);

	return true;
}

void UG2IGameSoundManager::RemoveAllSounds()
{
	for (auto Iterator = ActiveSounds.CreateIterator(); Iterator; ++Iterator)
	{
		if (Iterator.Value())
		{
			Iterator.Value()->OnAudioFinishedNative.RemoveAll(this);

			Iterator.Value()->Stop();
			Iterator.RemoveCurrent();
		}
	}

	ActiveSounds.Empty();
	IdStack.Empty();
	CurrentNumberAvailable = STARTSOUNDSTACKSIZE;
	for (int32 i = 0; i < CurrentNumberAvailable; ++i) {
		IdStack.Push(i);
	}
}

void UG2IGameSoundManager::UpdateStackSize() {
	for (int32 i = CurrentNumberAvailable; i < CurrentNumberAvailable * 2; ++i) {
		IdStack.Push(i);
	}
	CurrentNumberAvailable *= 2;
}

void UG2IGameSoundManager::InitGlobalAudio(USoundMix* _MainMix, const TMap<EG2ISoundType, USoundClass*>& _SoundClasses)
{
	MainSoundMix = _MainMix;
	
	SoundClasses.Empty();
	for (auto& Iterator : _SoundClasses)
	{
		SoundClasses.Add(Iterator.Key, Iterator.Value);
	}

	GlobalSoundMultipliers.Empty();
	GlobalSoundMultipliers.Add(EG2ISoundType::SpeechSound, 1.0f);
	GlobalSoundMultipliers.Add(EG2ISoundType::MusicSound, 1.0f);
	GlobalSoundMultipliers.Add(EG2ISoundType::EffectSound, 1.0f);
	GlobalSoundMultipliers.Add(EG2ISoundType::DefaultSound, 1.0f);

	UWorld* World = GetWorld();
	if (MainSoundMix && World)
	{
		UGameplayStatics::PushSoundMixModifier(World, MainSoundMix);
	}
	else {
		UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Can't get the World or MainSoundMix"), *GetName(), *FString(__FUNCTION__));
	}
}

void UG2IGameSoundManager::SetGlobalVolume(const EG2ISoundType SoundType, const float NewVolume)
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

	const float ClampedVolume = FMath::Clamp(NewVolume, 0.0f, 1.0f);

	GlobalSoundMultipliers[SoundType] = ClampedVolume;

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

float UG2IGameSoundManager::GetGlobalVolume(const EG2ISoundType SoundType) const {
	return GlobalSoundMultipliers[SoundType];
}

void UG2IGameSoundManager::OnSoundFinished(UAudioComponent* AudioComponent, const int32 SoundId)
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
float UG2IGameSoundManager::GetSoundVolume(const int32 SoundId) const
{
	if (const UAudioComponent* Component = GetAudioById(SoundId))
	{
		return Component->VolumeMultiplier;
	}

	UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Invalid SoundId: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
	return 0.0f;
}

float UG2IGameSoundManager::GetSoundPitch(const int32 SoundId) const
{
	if (const UAudioComponent* Component = GetAudioById(SoundId))
	{
		return Component->PitchMultiplier;
	}

	UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Invalid SoundId: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
	return 1.0f;
}

FVector UG2IGameSoundManager::GetSoundLocation(const int32 SoundId) const
{
	if (const UAudioComponent* Component = GetAudioById(SoundId))
	{
		return Component->GetComponentLocation();
	}

	UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Invalid SoundId: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
	return FVector::ZeroVector;
}

USceneComponent* UG2IGameSoundManager::GetSoundAttachment(const int32 SoundId) const
{
	if (const UAudioComponent* Component = GetAudioById(SoundId))
	{
		return Component->GetAttachParent();
	}

	UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Invalid SoundId: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
	return nullptr;
}

bool UG2IGameSoundManager::SetSoundAutoDestroy(const int32 SoundId, const bool bNewAutoDestroy)
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

bool UG2IGameSoundManager::GetSoundAutoDestroy(const int32 SoundId) const
{
	if (const UAudioComponent* Component = GetAudioById(SoundId))
	{
		return Component->bAutoDestroy;
	}

	UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Invalid SoundId: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
	return false;
}

bool UG2IGameSoundManager::GetSoundPlayingOneTime(const int32 SoundId) const
{
	if (const UAudioComponent* Component = GetAudioById(SoundId))
	{
		return Component->ComponentHasTag(OneTimeTag);
	}

	UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Invalid SoundId: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
	return false;
}

bool UG2IGameSoundManager::SetSoundPlayingOneTime(const int32 SoundId, const bool bNewIsPlayingOneTime)
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
	}
	else
	{
		Component->ComponentTags.Remove(OneTimeTag);

	}

	return true;
}

bool UG2IGameSoundManager::IsSoundPlaying(const int32 SoundId) const
{
	if (const TObjectPtr<UAudioComponent>* FoundAudioComp = ActiveSounds.Find(SoundId))
	{
		if (*FoundAudioComp)
		{
			return (*FoundAudioComp)->IsPlaying();
		}
	}
	return false;
}
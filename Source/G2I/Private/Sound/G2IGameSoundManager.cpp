#include "Sound/G2IGameSoundManager.h"
#include "G2I.h"
#include "Components/AudioComponent.h"
#include "Components/SceneComponent.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
#include "Kismet/GameplayStatics.h"
#include "G2IGameInstance.h"
#include "G2IOptionsParameters.h"
#include "TimerManager.h"

constexpr int32 START_SOUND_STACK_SIZE = 20;

TObjectPtr<UAudioComponent> UG2IGameSoundManager::GetAudioById(const int32 SoundId) const
{
	if (SoundId < 0) 
	{
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Wrong SoundId: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
		return nullptr;
	}

	if (const FG2IActiveSoundData* SoundData = ActiveSounds.Find(SoundId)) 
	{
		if (SoundData->AudioComponent) 
		{
			return SoundData->AudioComponent;
		}
	}

	UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: SoundId %d not found"), *GetName(), *FString(__FUNCTION__), SoundId);
	return nullptr;
}

FG2IActiveSoundData* UG2IGameSoundManager::GetSoundDataById(const int32 SoundId)
{
	return ActiveSounds.Find(SoundId);
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

	for (int32 i = 0; i < START_SOUND_STACK_SIZE; ++i)
	{
		IdStack.Push(i);
	}
	CurrentNumberAvailable = START_SOUND_STACK_SIZE;

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

	OptionsParameters = GameInstance->GetOptionsParameters();
	if (!OptionsParameters)
	{
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: OptionsParameters is null in GameInstance!"), *GetName(), *FString(__FUNCTION__));
	}

	OptionsParameters->MinVolume = FMath::Max(OptionsParameters->MinVolume, 0.f);

	OptionsParameters->MaxVolume = FMath::Max(OptionsParameters->MaxVolume, OptionsParameters->MinVolume);

	OptionsParameters->StepVolume = FMath::Clamp(OptionsParameters->StepVolume, 0.1f, OptionsParameters->MaxVolume - OptionsParameters->MinVolume);

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
					Iterator.Value / 10.f,
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
	if (const UWorld* World = GetWorld())
	{
		for (auto& Pair : ActiveSounds)
		{
			World->GetTimerManager().ClearTimer(Pair.Value.PlayTimerHandle);
		}
	}
	ActiveSounds.Empty();
	SoundPlayQueue.Empty();
	CurrentSoundFromQueueId = -1;

	IdStack.Empty();
	CurrentNumberAvailable = START_SOUND_STACK_SIZE;

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
	if (!NewSoundConfig.Sound) {
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
	AudioComponent->bAutoActivate = false;
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

	AudioComponent->SetVolumeMultiplier(NewSoundConfig.VolumeMultiplier / 10.f);
	AudioComponent->SetPitchMultiplier(NewSoundConfig.PitchMultiplier);

	const bool bAssetIsLooping = NewSoundConfig.Sound->IsLooping();

	AudioComponent->bAutoDestroy = bAssetIsLooping ? false : NewSoundConfig.bAutoDestroy;

	if (IdStack.IsEmpty()) {
		UpdateStackSize();
	}

	const int32 NewSoundId = IdStack.Pop();

	FG2IActiveSoundData NewSoundData;
	NewSoundData.AudioComponent = AudioComponent;
	NewSoundData.bIsPlayingOneTime = NewSoundConfig.bIsPlayingOneTime;
	NewSoundData.bIsPlayingFromStart = NewSoundConfig.bIsPlayingFromStart;
	NewSoundData.bIsPlayed = false;
	NewSoundData.Delay = NewSoundConfig.Delay;
	NewSoundData.bIsPlayingInQueue = NewSoundConfig.bIsPlayingInQueue;
	NewSoundData.bIsPausesQueuePlay = NewSoundConfig.bIsPausesQueuePlay;

	ActiveSounds.Add(NewSoundId, NewSoundData);
	AudioComponent->OnAudioFinishedNative.AddUObject(this, &UG2IGameSoundManager::OnSoundFinished, NewSoundId);
	return NewSoundId;
}

bool UG2IGameSoundManager::PlaySound(const int32 SoundId, const float FadeInTime)
{
	FG2IActiveSoundData* SoundData = ActiveSounds.Find(SoundId);
	if (!ensure(SoundData) || !ensure(SoundData->AudioComponent)) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: The sound manager can't get an audio component for ID: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
		return false;
	}

	if (SoundData->bIsPlayingOneTime && SoundData->bIsPlayed) {
		return false;
	}

	if (SoundData->bIsPlayingInQueue) {
		HandleSoundQueueAdd(SoundId);
		return true;
	}

	if (SoundData->bIsPlayingFromStart || !SoundData->AudioComponent->IsPlaying()) {
		SoundData->bIsPlayed = true;
		if (SoundData->Delay > 0.0f)
		{
			if (const UWorld* World = GetWorld())
			{
				FTimerDelegate TimerDel;
				TimerDel.BindUObject(this, &UG2IGameSoundManager::ExecuteDelayedPlay, SoundId, FadeInTime);

				World->GetTimerManager().SetTimer(SoundData->PlayTimerHandle, TimerDel, SoundData->Delay, false);
			}
			else {
				UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: The sound manager can't get the World for ID: %d, executing default play"), *GetName(), *FString(__FUNCTION__), SoundId);
				ExecuteDelayedPlay(SoundId, FadeInTime);
			}
		}
		else
		{
			ExecuteDelayedPlay(SoundId, FadeInTime);
		}
		return true;
	}

	return false;
}

void UG2IGameSoundManager::ExecuteDelayedPlay(const int32 SoundId, const float FadeInTime)
{
	if (FG2IActiveSoundData* SoundData = ActiveSounds.Find(SoundId))
	{
		if (SoundData->AudioComponent)
		{
			SoundData->AudioComponent->FadeIn(FadeInTime, SoundData->AudioComponent->VolumeMultiplier);
		}
		SoundData->PlayTimerHandle.Invalidate();
	}
}

bool UG2IGameSoundManager::StopSound(const int32 SoundId, const float FadeOutTime)
{
	FG2IActiveSoundData* SoundData = ActiveSounds.Find(SoundId);
	if (!ensure(SoundData) || !ensure(SoundData->AudioComponent)) {
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: The sound manager can't get an audio component for ID: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
		return false;
	}

	if (const UWorld* World = GetWorld()) {
		World->GetTimerManager().ClearTimer(SoundData->PlayTimerHandle);
	}

	SoundData->AudioComponent->FadeOut(FadeOutTime, 0.0f);
	return true;
}

void UG2IGameSoundManager::StopAllSounds()
{
	for (const auto& Pair : ActiveSounds)
	{
		if (ensure(Pair.Value.AudioComponent))
		{
			Pair.Value.AudioComponent->Stop();
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

	NewVolume = FMath::Clamp(NewVolume, 0.0f, 10.0f);
	Component->SetVolumeMultiplier(NewVolume / 10.f);

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
	SoundPlayQueue.Empty();
	CurrentSoundFromQueueId = -1;

	for (auto Iterator = ActiveSounds.CreateIterator(); Iterator; ++Iterator)
	{
		if (Iterator.Value().AudioComponent)
		{
			Iterator.Value().AudioComponent->OnAudioFinishedNative.RemoveAll(this);

			Iterator.Value().AudioComponent->Stop();

			if (const UWorld* World = GetWorld())
			{
				World->GetTimerManager().ClearTimer(Iterator.Value().PlayTimerHandle);
			}
		}
		Iterator.RemoveCurrent();
	}
	ActiveSounds.Empty();
	IdStack.Empty();

	CurrentNumberAvailable = START_SOUND_STACK_SIZE;
	for (int32 i = 0; i < CurrentNumberAvailable; ++i)
	{
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

	if (OptionsParameters)
	{
		GlobalSoundMultipliers.Add(EG2ISoundType::SpeechSound, OptionsParameters->DefaultDialoguesVolume);
		GlobalSoundMultipliers.Add(EG2ISoundType::MusicSound, OptionsParameters->DefaultMusicVolume);
		GlobalSoundMultipliers.Add(EG2ISoundType::EffectSound, OptionsParameters->DefaultEffectsVolume);
		GlobalSoundMultipliers.Add(EG2ISoundType::DefaultSound, OptionsParameters->DefaultCommonVolume);
	}
	else
	{
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: OptionsParameters is null! Using default volume 10.0f"), *GetName(), *FString(__FUNCTION__));
		GlobalSoundMultipliers.Add(EG2ISoundType::SpeechSound, 10.0f);
		GlobalSoundMultipliers.Add(EG2ISoundType::MusicSound, 10.0f);
		GlobalSoundMultipliers.Add(EG2ISoundType::EffectSound, 10.0f);
		GlobalSoundMultipliers.Add(EG2ISoundType::DefaultSound, 10.0f);
	}

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
	if (!ensure(OptionsParameters)) {
		UE_LOG(LogG2I, Error, TEXT("[%s][%s]: Can't get OptionParameters for the SoundType (ID: %d)"), *GetName(), *FString(__FUNCTION__), static_cast<int32>(SoundType));
		return;
	}
	const float ClampedVolume = FMath::Clamp(NewVolume, OptionsParameters->MinVolume, OptionsParameters->MaxVolume);

	if (float* VolumePtr = GlobalSoundMultipliers.Find(SoundType))
	{
		*VolumePtr = ClampedVolume;
	}
	else
	{
		GlobalSoundMultipliers.Add(SoundType, ClampedVolume);
	}

	if (OptionsParameters)
	{
		switch (SoundType)
		{
		case EG2ISoundType::SpeechSound:
			OptionsParameters->DialoguesVolume = ClampedVolume;
			break;
		case EG2ISoundType::MusicSound:
			OptionsParameters->MusicVolume = ClampedVolume;
			break;
		case EG2ISoundType::EffectSound:
			OptionsParameters->EffectsVolume = ClampedVolume;
			break;
		case EG2ISoundType::DefaultSound:
			OptionsParameters->CommonVolume = ClampedVolume;
			break;
		}
	}

	if (UWorld* World = GetWorld()) {
		UGameplayStatics::SetSoundMixClassOverride(
			World,
			MainSoundMix,
			TargetClass,
			ClampedVolume / 10.f,
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
	if (!AudioComponent)
	{
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Can't get AudioComponent when sound with ID %d finished"), *GetName(), *FString(__FUNCTION__), SoundId);
		ActiveSounds.Remove(SoundId);
		IdStack.Push(SoundId);
		return;
	}
	if (AudioComponent->bAutoDestroy) {
		ActiveSounds.Remove(SoundId);
		IdStack.Push(SoundId);
		
		if (FG2IActiveSoundData* SoundData = ActiveSounds.Find(SoundId))
		{
			if (const UWorld* World = GetWorld())
			{
				World->GetTimerManager().ClearTimer(SoundData->PlayTimerHandle);
			}
		}
	}

	const bool bAdvanceQueue = (SoundId == CurrentSoundFromQueueId) && (SoundId != -1);
	if (!bAdvanceQueue)
	{
		return;
	}

	

	SoundPlayQueue.Remove(SoundId);

	if (bAdvanceQueue)
	{
		CurrentSoundFromQueueId = -1;
		AdvanceQueue();
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
	if (const FG2IActiveSoundData* SoundData = ActiveSounds.Find(SoundId))
	{
		return SoundData->bIsPlayingOneTime;
	}
	UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Invalid SoundId: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
	return false;
}

bool UG2IGameSoundManager::SetSoundPlayingOneTime(const int32 SoundId, const bool bNewIsPlayingOneTime)
{
	if (FG2IActiveSoundData* SoundData = ActiveSounds.Find(SoundId))
	{
		SoundData->bIsPlayingOneTime = bNewIsPlayingOneTime;
		return true;
	}
	UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Invalid SoundId: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
	return false;
}

bool UG2IGameSoundManager::IsSoundPlaying(const int32 SoundId) const
{
	if (const FG2IActiveSoundData* SoundData = ActiveSounds.Find(SoundId))
	{
		if (SoundData->AudioComponent) {
			return SoundData->AudioComponent->IsPlaying();
		}
	}
	UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Invalid SoundId: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
	return false;
}

bool UG2IGameSoundManager::GetSoundPlayingFromStart(const int32 SoundId) const
{
	if (const FG2IActiveSoundData* SoundData = ActiveSounds.Find(SoundId))
	{
		return SoundData->bIsPlayingFromStart;
	}
	
	UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Invalid SoundId: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
	return false;
}

bool UG2IGameSoundManager::SetSoundPlayingFromStart(const int32 SoundId, const bool bNewIsPlayingFromStart)
{
	if (FG2IActiveSoundData* SoundData = ActiveSounds.Find(SoundId))
	{
		SoundData->bIsPlayingFromStart = bNewIsPlayingFromStart;
		return true;
	}
	UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Invalid SoundId: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
	return false;
}

float UG2IGameSoundManager::GetSoundDelay(const int32 SoundId) const
{
	if (const FG2IActiveSoundData* SoundData = ActiveSounds.Find(SoundId))
	{
		return SoundData->Delay;
	}
	UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Invalid SoundId: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
	return 0.0f;
}

bool UG2IGameSoundManager::SetSoundDelay(const int32 SoundId, const float NewDelay)
{
	if (FG2IActiveSoundData* SoundData = ActiveSounds.Find(SoundId))
	{
		SoundData->Delay = NewDelay;
		return true;
	}
	UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Invalid SoundId: %d"), *GetName(), *FString(__FUNCTION__), SoundId);
	return false;
}

void UG2IGameSoundManager::HandleSoundQueueAdd(const int32 NewSoundId)
{
	FG2IActiveSoundData* NewSoundData = ActiveSounds.Find(NewSoundId);
	if (!ensure(NewSoundData) || !ensure(NewSoundData->AudioComponent))
	{
		UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Invalid SoundId: %d or can't get Audio Component"), *GetName(), *FString(__FUNCTION__), NewSoundId);
		return;
	}

	NewSoundData->bIsPlayed = true;

	FG2IActiveSoundData* CurrentQueueSound = ActiveSounds.Find(CurrentSoundFromQueueId);

	if (CurrentQueueSound && CurrentQueueSound->AudioComponent && CurrentQueueSound->AudioComponent->IsPlaying())
	{
		switch (NewSoundData->bIsPausesQueuePlay)
		{
		case EG2ISoundStopType::Pause:
		{
			CurrentQueueSound->AudioComponent->SetPaused(true);
			SoundPlayQueue.Insert(CurrentSoundFromQueueId, 0);
			CurrentSoundFromQueueId = NewSoundId;
			break;
		}
		case EG2ISoundStopType::Stop:
		{
			CurrentQueueSound->AudioComponent->OnAudioFinishedNative.RemoveAll(this);
			CurrentQueueSound->AudioComponent->Stop();

			if (const UWorld* World = GetWorld())
			{
				World->GetTimerManager().ClearTimer(CurrentQueueSound->PlayTimerHandle);
			}
			ActiveSounds.Remove(CurrentSoundFromQueueId);
			IdStack.Push(CurrentSoundFromQueueId);

			CurrentSoundFromQueueId = NewSoundId;
			break;
		}
		case EG2ISoundStopType::Not:
		default:
		{
			SoundPlayQueue.Add(NewSoundId);
			return;
		}
		}
	}
	else
	{
		CurrentSoundFromQueueId = NewSoundId;
	}

	if (NewSoundData->Delay > 0.0f)
	{
		if (const UWorld* World = GetWorld())
		{
			FTimerDelegate TimerDel;
			TimerDel.BindUObject(this, &UG2IGameSoundManager::ExecuteDelayedPlay, NewSoundId, 0.0f);
			World->GetTimerManager().SetTimer(NewSoundData->PlayTimerHandle, TimerDel, NewSoundData->Delay, false);
		}
		else
		{
			ExecuteDelayedPlay(NewSoundId, 0.0f);
		}
	}
	else
	{
		ExecuteDelayedPlay(NewSoundId, 0.0f);
	}
}

void UG2IGameSoundManager::AdvanceQueue()
{
	while (SoundPlayQueue.Num() > 0 && CurrentSoundFromQueueId == -1)
	{
		const int32 NextSoundId = SoundPlayQueue[0];
		SoundPlayQueue.RemoveAt(0);


		FG2IActiveSoundData* NextSound = ActiveSounds.Find(NextSoundId);
		if (!NextSound)
		{
			UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Invalid SoundId: %d"), *GetName(), *FString(__FUNCTION__), NextSoundId);
			continue;
		}

		if (!NextSound->AudioComponent)
		{
			UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: Can't get AudioComponent"), *GetName(), *FString(__FUNCTION__));
			continue;
		}

		CurrentSoundFromQueueId = NextSoundId;

		if (NextSound->AudioComponent->bIsPaused)
		{
			NextSound->AudioComponent->SetPaused(false);
		}
		else
		{
			NextSound->AudioComponent->Play();
		}
	}
}
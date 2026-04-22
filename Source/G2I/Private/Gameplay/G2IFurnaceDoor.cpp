#include "Gameplay/G2IFurnaceDoor.h"
#include "TimerManager.h"
#include "G2I.h"
#include "Sound/G2ISoundComponent.h"

void AG2IFurnaceDoor::StartTick()
{
	CurrentTicks = 0;
	
	if (MaxTicks <= 0) {
		return;
	}

	const UWorld* World = GetWorld();
	if (!ensure(World)) {
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}

	const float Interval = TotalTime / MaxTicks;

	World->GetTimerManager().SetTimer(
		PushTimerHandle,
		this,
		&AG2IFurnaceDoor::PerformTick,
		Interval,
		true
	);
}

void AG2IFurnaceDoor::PerformTick()
{
	if (SoundComp && !SoundComp->IsSoundPlaying(FurnaceDoorOpenSoundId)) {
		SoundComp->PlaySound(FurnaceDoorOpenSoundId);
	}

	OnPushing_Implementation(Force);

	CurrentTicks++;

	if (CurrentTicks < MaxTicks)
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (!ensure(World)) {
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}

	World->GetTimerManager().ClearTimer(PushTimerHandle);
}

void AG2IFurnaceDoor::Activate_Implementation()
{
	StartTick();
}

void AG2IFurnaceDoor::Deactivate_Implementation() {
	return;
}

void AG2IFurnaceDoor::BeginPlay() {
	Super::BeginPlay();

	if (SoundComp && SoundComp->SetupSounds.Contains(TEXT("FurnaceDoorOpenSound")))
	{
		FurnaceDoorOpenSoundId = SoundComp->AddSound(SoundComp->SetupSounds[TEXT("FurnaceDoorOpenSound")]);
		if (FurnaceDoorOpenSoundId == -1)
		{
			UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: FurnaceDoorOpenSoundId (ID == -1)"), *GetName(), *FString(__FUNCTION__));
		}
	}
}

AG2IFurnaceDoor::AG2IFurnaceDoor() {
	SoundComp = CreateDefaultSubobject<UG2ISoundComponent>(TEXT("SoundComponent"));
	if (SoundComp) {
		SoundComp->SetupSounds.Add(TEXT("FurnaceDoorOpenSound"), FSoundConfig());
	}

	if (SoundComp) {
		FSoundConfig DefaultConfig;
		if (RootComponent) {
			DefaultConfig.AttachToComponent.ComponentProperty = RootComponent->GetFName();
		}
		static ConstructorHelpers::FObjectFinder<USoundWave> SoundAsset(TEXT("/Script/Engine.SoundWave'/Game/G2I_Game/Audio/Sounds/SoundRaw/SW_FurnaceDoorOpenSound.SW_FurnaceDoorOpenSound'"));
		if (SoundAsset.Succeeded()) {
			DefaultConfig.Sound = SoundAsset.Object;
		}

		SoundComp->SetupSounds.Add(TEXT("FurnaceDoorOpenSound"), DefaultConfig);
	}
}
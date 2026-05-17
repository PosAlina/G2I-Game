
#include "Gameplay/G2IActivationOrderManagerSounds.h"
#include "Kismet/GameplayStatics.h"
#include "G2ISoundComponent.h"
#include "G2I.h"

AG2IActivationOrderManagerSounds::AG2IActivationOrderManagerSounds()
{
	SoundComp = CreateDefaultSubobject<UG2ISoundComponent>(TEXT("SoundComponent"));
	if (SoundComp) 
	{
		SoundComp->SetupSounds.Add(CorrectSoundName, FSoundConfig());
		SoundComp->SetupSounds.Add(FailedSoundName, FSoundConfig());
	}
}

void AG2IActivationOrderManagerSounds::BeginPlay()
{
    Super::BeginPlay();

    if (!ensure(SoundComp)) {
        UE_LOG(LogG2I, Warning, TEXT("Sound Component is not set for %s"), *GetName());
        return;
    }

    if (SoundComp->SetupSounds.Contains(CorrectSoundName))
    {
		CorrectSoundId = SoundComp->AddSound(SoundComp->SetupSounds[CorrectSoundName]);
        if (CorrectSoundId == -1)
        {
            UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: CorrectSoundId (ID == -1)"), *GetName(), *FString(__FUNCTION__));
        }
    }

	if (SoundComp->SetupSounds.Contains(FailedSoundName))
	{
		FailedSoundId = SoundComp->AddSound(SoundComp->SetupSounds[FailedSoundName]);
		if (FailedSoundId == -1)
		{
			UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: FailedSoundId (ID == -1)"), *GetName(), *FString(__FUNCTION__));
		}
	}
}


void AG2IActivationOrderManagerSounds::OrderCompleted()
{
	Super::OrderCompleted();

	UG2ISoundComponent::PlaySoundSafe(SoundComp, CorrectSoundId);
}

void AG2IActivationOrderManagerSounds::OrderFailed()
{
	Super::OrderFailed();

	UG2ISoundComponent::PlaySoundSafe(SoundComp, FailedSoundId);
}

void AG2IActivationOrderManagerSounds::OnActorActivated(AActor* ActivatedActor, bool bReactivation, UG2IActivationWithOrderComponent* ActivationComponent)
{
	if (bReactivation && bAllowCancel)
	{
		if (CurrentIndex > 0 && ActivatedActor == ActivatedActorsArray.Last())
		{
			ActivatedActorsArray.Pop();
			CurrentIndex--;
			if (ActivationComponent)
				ActivationComponent->Declined();
			return;
		}
	}

	ActivatedActorsArray.Add(ActivatedActor);
	if (ActivationComponent)
		ActivationComponent->Accepted(CurrentIndex);
	CurrentIndex++;
	CheckIfOrderCompleted();
}

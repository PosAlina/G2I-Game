#include "Gameplay/G2ITogglePlatform.h"
#include "Components/TimelineComponent.h"
#include "G2I.h"
#include "G2ISoundComponent.h"

AG2ITogglePlatform::AG2ITogglePlatform() 
{
    SoundComp = CreateDefaultSubobject<UG2ISoundComponent>(TEXT("SoundComponent"));
    if (SoundComp) {
        SoundComp->SetupSounds.Add(PlatformSoundName, FSoundConfig());
    }
}

void AG2ITogglePlatform::BeginPlay()
{
    Super::BeginPlay();

    if (!ensure(PlatformMesh))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: PlatformMesh is NULL"), *GetActorNameOrLabel());
        return;
    }

    if (bIsActivated)
    {
        Timeline->SetPlayRate(1.0f);
        Timeline->PlayFromStart();
    }
    else
    {
        PlatformMesh->SetWorldLocation(StartLoc);
        Timeline->SetPlaybackPosition(0.0f, false);
    }

    if (!ensure(SoundComp)) {
        UE_LOG(LogG2I, Warning, TEXT("Sound Component is not set for %s"), *GetName());
        return;
    }

    if (SoundComp->SetupSounds.Contains(PlatformSoundName))
    {
        PlatformSoundId = SoundComp->AddSound(SoundComp->SetupSounds[PlatformSoundName]);
        if (PlatformSoundId == -1)
        {
            UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: PlatformSoundId (ID == -1)"), *GetName(), *FString(__FUNCTION__));
        }
    }
}

void AG2ITogglePlatform::OnTimelineFinished()
{
    if (!ensure(Timeline))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Timeline is NULL"), *GetActorNameOrLabel());
        return;
    }
    Timeline->Stop();

    if (!ensure(SoundComp)) {
        UE_LOG(LogG2I, Warning, TEXT("Sound Component is not set for %s"), *GetName());
        return;
    }
    SoundComp->StopSound(PlatformSoundId);
}

void AG2ITogglePlatform::ToggleLockingPlatform(UG2ILauncherComponent* LauncherComponent, AActor* ComponentOwner,
    const bool bIsLocked)
{
    Super::ToggleLockingPlatform(LauncherComponent, ComponentOwner, bIsLocked);
    
    if (bIsLocked && bIsActivated)
    {
        Deactivate_Implementation();
        return;
    }
    if (!bIsLocked && !bIsActivated)
    {
        Activate_Implementation();
        return;
    }
}

void AG2ITogglePlatform::Activate_Implementation()
{
    if (bIsActivated) return;
    bIsActivated = true;

    if (!ensure(Timeline))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Timeline is NULL"), *GetActorNameOrLabel());
        return;
    }

    if (Timeline->IsPlaying())
    {
        bMovingToEnd = true;
        Timeline->SetPlayRate(1.0f);
    }
    else
    {
        bMovingToEnd = true;
        Timeline->SetPlayRate(1.0f);
        Timeline->Play();
    }
    if (!ensure(SoundComp)) {
        UE_LOG(LogG2I, Warning, TEXT("Sound Component is not set for %s"), *GetName());
        return;
    }
    SoundComp->PlaySound(PlatformSoundId);
}

void AG2ITogglePlatform::Deactivate_Implementation()
{
    if (!bIsActivated) return;
    bIsActivated = false;

    if (!ensure(Timeline))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Timeline is NULL"), *GetActorNameOrLabel());
        return;
    }

    if (Timeline->IsPlaying())
    {
        bMovingToEnd = false;
        Timeline->SetPlayRate(-1.0f);
    }
    else
    {
        bMovingToEnd = false;
        Timeline->SetPlayRate(-1.0f);
        Timeline->Play();
    }
    if (!ensure(SoundComp)) {
        UE_LOG(LogG2I, Warning, TEXT("Sound Component is not set for %s"), *GetName());
        return;
    }
    SoundComp->PlaySound(PlatformSoundId);
}
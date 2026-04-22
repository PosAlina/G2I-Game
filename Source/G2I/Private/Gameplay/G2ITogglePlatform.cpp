#include "Gameplay/G2ITogglePlatform.h"
#include "Components/TimelineComponent.h"
#include "G2I.h"


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
}

void AG2ITogglePlatform::OnTimelineFinished()
{
    if (!ensure(Timeline))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Timeline is NULL"), *GetActorNameOrLabel());
        return;
    }
    Timeline->Stop();
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
}
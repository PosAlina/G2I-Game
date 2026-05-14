#include "Gameplay/G2IPingPongPlatform.h"
#include "Components/TimelineComponent.h"
#include "G2I.h"

void AG2IPingPongPlatform::BeginPlay()
{
    Super::BeginPlay();
    
    if (bIsActivated)
    {
        Activate_Implementation();
    }
    else
    {
        Deactivate_Implementation();
    }
}

void AG2IPingPongPlatform::OnTimelineFinished()
{
    if (!ensure(Timeline))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Timeline is NULL"), *GetActorNameOrLabel());
        return;
    }

    if (bMovingForward)
    {
        bMovingForward = false;
        Timeline->SetPlayRate(-1.0f);
    }
    else
    {
        bMovingForward = true;
        Timeline->SetPlayRate(1.0f);
    }
    Timeline->Play();
}

void AG2IPingPongPlatform::ToggleLockingPlatform(
    UG2ILauncherComponent* LauncherComponent, AActor* ComponentOwner, const bool bIsLocked)
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
    }
}

void AG2IPingPongPlatform::Activate_Implementation()
{
    if (!ensure(Timeline))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Timeline is NULL"), *GetActorNameOrLabel());
        return;
    }

    Timeline->SetPlayRate(1.0f);
    Timeline->PlayFromStart();
    
    bIsActivated = true;
}

void AG2IPingPongPlatform::Deactivate_Implementation()
{
    
    Timeline->Stop();
    bIsActivated = false;
}

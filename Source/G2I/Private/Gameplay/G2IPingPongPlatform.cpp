#include "Gameplay/G2IPingPongPlatform.h"
#include "Components/TimelineComponent.h"
#include "G2I.h"

void AG2IPingPongPlatform::BeginPlay()
{
    Super::BeginPlay();

    if (!ensure(Timeline))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Timeline is NULL"), *GetActorNameOrLabel());
        return;
    }

    Timeline->SetPlayRate(1.0f);
    Timeline->PlayFromStart();
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
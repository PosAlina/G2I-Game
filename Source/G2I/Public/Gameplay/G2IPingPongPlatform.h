#pragma once

#include "CoreMinimal.h"
#include "Gameplay/G2IBaseMovingPlatform.h"
#include "G2IPingPongPlatform.generated.h"

UCLASS()
class G2I_API AG2IPingPongPlatform : public AG2IBaseMovingPlatform
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
    virtual void OnTimelineFinished() override;

private:
    bool bMovingForward = true;
};
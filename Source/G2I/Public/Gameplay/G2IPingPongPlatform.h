#pragma once

#include "CoreMinimal.h"
#include "G2IActivationInterface.h"
#include "Gameplay/G2IBaseMovingPlatform.h"
#include "G2IPingPongPlatform.generated.h"

UCLASS()
class G2I_API AG2IPingPongPlatform : public AG2IBaseMovingPlatform, public IG2IActivationInterface
{
    GENERATED_BODY()
    
public:
    virtual void Activate_Implementation() override;
    virtual void Deactivate_Implementation() override;

protected:
    virtual void BeginPlay() override;
    virtual void OnTimelineFinished() override;

    virtual void ToggleLockingPlatform(
        UG2ILauncherComponent* LauncherComponent, AActor* ComponentOwner, bool bIsLocked) override;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsActivated = true;

private:
    bool bMovingForward = true;
};
#pragma once

#include "CoreMinimal.h"
#include "Gameplay/G2IBaseMovingPlatform.h"
#include "G2IActivationInterface.h"
#include "G2ITogglePlatform.generated.h"

UCLASS()
class G2I_API AG2ITogglePlatform : public AG2IBaseMovingPlatform, public IG2IActivationInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Movement")
    virtual void Activate_Implementation() override;

    UFUNCTION(BlueprintCallable, Category = "Movement")
    virtual void Deactivate_Implementation() override;

protected:
    virtual void BeginPlay() override;
    virtual void OnTimelineFinished() override;

private:
    bool bIsActivated = false;
    bool bMovingToEnd = true;
};
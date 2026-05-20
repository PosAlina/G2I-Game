#pragma once

#include "CoreMinimal.h"
#include "Gameplay/G2IBaseMovingPlatform.h"
#include "G2IActivationInterface.h"
#include "G2ITogglePlatform.generated.h"

class UG2ILauncherComponent;
class UG2ISoundComponent;
UCLASS()
class G2I_API AG2ITogglePlatform : public AG2IBaseMovingPlatform, public IG2IActivationInterface
{
    GENERATED_BODY()

public:
    AG2ITogglePlatform();

    UFUNCTION(BlueprintCallable, Category = "Movement")
    virtual void Activate_Implementation() override;

    UFUNCTION(BlueprintCallable, Category = "Movement")
    virtual void Deactivate_Implementation() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName PlatformSoundName = FName("PlatformSound");

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound")
    TObjectPtr<UG2ISoundComponent> SoundComp;

protected:
    int32 PlatformSoundId = -1;

    virtual void BeginPlay() override;
    virtual void OnTimelineFinished() override;

    virtual void ToggleLockingPlatform(
        UG2ILauncherComponent* LauncherComponent, AActor* ComponentOwner, bool bIsLocked) override;

private:
    bool bIsActivated = false;
    bool bMovingToEnd = true;
};
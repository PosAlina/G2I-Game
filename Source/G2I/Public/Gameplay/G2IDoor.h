#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "G2IActivationInterface.h"
#include "G2IDoor.generated.h"

class UG2ILauncherComponent;
class UStaticMeshComponent;

UCLASS()
class G2I_API AG2IDoor : public AActor, public IG2IActivationInterface
{
    GENERATED_BODY()

public:
    AG2IDoor();

    UFUNCTION(BlueprintCallable, Category = "Door")
    void RotatingDoor();

protected:

    virtual void BeginPlay() override;
    
    UFUNCTION()
    void ToggleLockingDoor(UG2ILauncherComponent* LauncherComponent, AActor* ComponentOwner, bool bIsLocked);

    float StartAngle;

public:
    UPROPERTY()
    TObjectPtr<UG2ILauncherComponent> LauncherComp;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
    TObjectPtr<UStaticMeshComponent> DoorBaseComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
    float OpenAngle = 90.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Door")
    bool bIsDoorMoving = false;

    UPROPERTY(BlueprintReadOnly, Category = "Door")
    bool bIsOpen = false;

    UPROPERTY(BlueprintReadOnly, Category = "Door")
    bool bIsFinished = false;

    UPROPERTY()
    TObjectPtr<UTimelineComponent> DoorTimeline;

    UPROPERTY(EditDefaultsOnly, Category = "Door")
    TObjectPtr<UCurveFloat> DoorCurve;

    UFUNCTION()
    void OnTimelineUpdate(float Alpha);

    UFUNCTION()
    void OnTimelineFinished();

    virtual void Activate_Implementation() override;
    virtual void Deactivate_Implementation() override;

};
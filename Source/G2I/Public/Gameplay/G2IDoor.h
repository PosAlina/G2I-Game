#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "G2IActivationInterface.h"
#include "G2IDoor.generated.h"

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

    float StartAngle;

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
    TObjectPtr<UStaticMeshComponent> DoorBaseComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
    float OpenAngle = 90.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Door")
    bool bIsDoorMoving;

    UPROPERTY(BlueprintReadOnly, Category = "Door")
    bool bOpening;

    UPROPERTY(BlueprintReadOnly, Category = "Door")
    bool bIsFinished;

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
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "G2IBaseMovingPlatform.generated.h"

class UArrowComponent;
class UStaticMeshComponent;
class UCurveFloat;

UCLASS(Abstract)
class G2I_API AG2IBaseMovingPlatform : public AActor
{
    GENERATED_BODY()

public:
    AG2IBaseMovingPlatform();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> PlatformMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UArrowComponent> StartPosition;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UArrowComponent> EndPosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    TObjectPtr<UCurveFloat> MovementCurve;

    UPROPERTY()
    TObjectPtr<UTimelineComponent> Timeline;

    FVector StartLoc;
    FVector EndLoc;

    FOnTimelineFloat ProgressFunction;

    UFUNCTION()
    virtual void HandleProgress(float Value);

    UFUNCTION()
    virtual void OnTimelineFinished() {};
};
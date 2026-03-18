#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "G2IPressureGauge.generated.h"

USTRUCT(BlueprintType)
struct FArrowInfo
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arrow")
    TObjectPtr<USceneComponent> ArrowComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow", meta = (ClampMin = "0.0", ClampMax = "360.0", UIMin = "0.0", UIMax = "360.0"))
    float TargetPitch = 0.f;

    UPROPERTY()
    float CurrentPitch = 0.f;
};

USTRUCT()
struct FActiveMovement
{
    GENERATED_BODY()

    TWeakObjectPtr<USceneComponent> Component;

    float StartPitch = 0.f;
    float TargetPitch = 0.f;
    float MoveTime = 0.f;
    float ElapsedTime = 0.f;
};

UCLASS()
class G2I_API AG2IPressureGauge : public AActor
{
    GENERATED_BODY()

public:
    AG2IPressureGauge();

protected:

    virtual void Tick(float DeltaTime) override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gauge")
    TArray<FArrowInfo> Arrows;

    UFUNCTION(BlueprintCallable, Category = "Gauge")
    void ArrowsAngle(bool bIsOn, const TArray<float>& AngleDeltas);

    UFUNCTION(BlueprintCallable, Category = "Gauge")
    bool CheckTargetAngles(float Tolerance = 1.0f) const;

    UFUNCTION(BlueprintCallable, Category = "Gauge")
    void InitializeArrowsComponent(TArray<USceneComponent*> InArrows);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSet<TObjectPtr<AActor>> PuzzleActors;

private:
    void SmoothMoveComponent(USceneComponent* Component, float StartPitch, float TargetPitch, float MoveTime = 1.0f);

    UPROPERTY()
    TArray<FActiveMovement> ActiveMovements;
};
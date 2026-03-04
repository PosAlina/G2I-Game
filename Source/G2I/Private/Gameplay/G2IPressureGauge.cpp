#include "Gameplay/G2IPressureGauge.h"
#include "Components/SceneComponent.h"
#include "G2I.h"

AG2IPressureGauge::AG2IPressureGauge()
{
    PrimaryActorTick.bCanEverTick = true;
    SetActorTickEnabled(false);
}

void AG2IPressureGauge::InitializeArrowsComponent(TArray<USceneComponent*> InArrows)
{
    if (!ensure(InArrows.Num() == Arrows.Num()))
    {
        UE_LOG(LogG2I, Warning, TEXT("Initializing of arrows is crashed in %s"), *GetActorNameOrLabel());
        return;
    }
    for (int32 i = 0; i < Arrows.Num(); i++)
    {
        Arrows[i].ArrowComponent = InArrows[i];
    }
}

void AG2IPressureGauge::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    for (int32 i = ActiveMovements.Num() - 1; i >= 0; --i)
    {
        FActiveMovement& Movement = ActiveMovements[i];
        USceneComponent* Comp = Movement.Component.Get();

        if (!Comp)
        {
            UE_LOG(LogG2I, Error, TEXT("Gauge '%s': ActiveMovement[%d] has null component, removing movement."), *GetName(), i);

            ActiveMovements.RemoveAt(i);
            continue;
        }

        Movement.ElapsedTime += DeltaTime;

        float Alpha = FMath::Clamp(Movement.ElapsedTime / Movement.MoveTime, 0.0f, 1.0f);

        float CurrentPitch = FMath::Lerp(Movement.StartPitch, Movement.TargetPitch, Alpha);

        float NormalizedPitch = FRotator::NormalizeAxis(CurrentPitch);
        Comp->SetRelativeRotation(FRotator(NormalizedPitch, 0.0f, 0.0f));

        if (Alpha >= 1.0f)
        {
            ActiveMovements.RemoveAt(i);
        }
    }

    if (ActiveMovements.Num() == 0)
    {
        SetActorTickEnabled(false);
    }
}

void AG2IPressureGauge::SmoothMoveComponent(USceneComponent* Component, float StartPitch, float TargetPitch, float MoveTime)
{
    if (!Component)
    {
        UE_LOG(LogG2I, Error, TEXT("Gauge '%s': SmoothMoveComponent with null component"), *GetName());
        return;
    }

    if (MoveTime <= 0.0f)
    {
        float NormalizedPitch = FRotator::NormalizeAxis(TargetPitch);
        Component->SetRelativeRotation(FRotator(NormalizedPitch, 0.0f, 0.0f));
        return;
    }

    FActiveMovement Movement;
    Movement.Component = Component;
    Movement.StartPitch = StartPitch;
    Movement.TargetPitch = TargetPitch;
    Movement.MoveTime = MoveTime;
    Movement.ElapsedTime = 0.0f;

    ActiveMovements.Add(Movement);

    SetActorTickEnabled(true);
}

void AG2IPressureGauge::ArrowsAngle(bool bIsOn, const TArray<float>& AngleDeltas)
{
    const float Direction = bIsOn ? 1.0f : -1.0f;

    const int32 Num = FMath::Min(Arrows.Num(), AngleDeltas.Num());

    for (int32 Index = 0; Index < Num; ++Index)
    {
        FArrowInfo& Info = Arrows[Index];

        if (!Info.ArrowComponent)
            continue;

        const float PreviousPitch = Info.CurrentPitch;

        Info.CurrentPitch += AngleDeltas[Index] * Direction;

        SmoothMoveComponent(Info.ArrowComponent, PreviousPitch, Info.CurrentPitch, 1.0f);
    }
}

bool AG2IPressureGauge::CheckTargetAngles(float Tolerance) const
{
    for (const FArrowInfo& Info : Arrows)
    {
        if (!Info.ArrowComponent)
            return false;

        float Current = FMath::Fmod(Info.CurrentPitch, 360.0f);
        if (Current < 0.0f)
            Current += 360.0f;

        float Target = Info.TargetPitch;

        float Diff = FMath::Abs(Current - Target);
        if (Diff > 180.0f)
            Diff = 360.0f - Diff;

        if (!FMath::IsNearlyZero(Diff, Tolerance))
            return false;
    }

    return true;
}
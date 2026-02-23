


#include "Gameplay/G2IPressureGaugeValve.h"
#include "Components/StaticMeshComponent.h"
#include "Curves/CurveFloat.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "G2I.h"

AG2IPressureGaugeValve::AG2IPressureGaugeValve()
{
	PrimaryActorTick.bCanEverTick = true;

	ValveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ValveMesh"));
	RootComponent = ValveMesh;

	ValveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ValveTimeline"));

}

void AG2IPressureGaugeValve::BeginPlay()
{
    Super::BeginPlay();

    bIsAState = false;
    bCanOpen = true;
    bIsRotating = false;

    if (ValveTimeline && ValveCurve)
    {
        FOnTimelineFloat UpdateFunction;
        UpdateFunction.BindUFunction(this, FName("OnTimelineUpdate"));
        ValveTimeline->AddInterpFloat(ValveCurve, UpdateFunction);

        FOnTimelineEvent FinishedFunction;
        FinishedFunction.BindUFunction(this, FName("OnTimelineFinished"));
        ValveTimeline->SetTimelineFinishedFunc(FinishedFunction);
        if (ValveMesh)
        { 
            ValveTimeline->SetTimelineLength(1.0f);

            ValveMesh->SetRelativeRotation(FRotator::ZeroRotator);
        }
    }
}

void AG2IPressureGaugeValve::ToggleValve()
{
    if (bIsRotating) return; 

    bIsAState = !bIsAState;

    if (bIsAState)
    {
        OpenValve();
    }
    else
    {
        CloseValve();
    }
}

void AG2IPressureGaugeValve::OpenValve()
{
    if (!ValveCurve)
    {
        UE_LOG(LogG2I, Warning, TEXT("ValveCurve is nullptr in %s"), *GetName());
        return;
    }

   if (ValveTimeline)
   {
       bIsRotating = true;
       bIsValveOpen = true;
       ValveTimeline->PlayFromStart();
   }
}

void AG2IPressureGaugeValve::CloseValve()
{
    if (!ValveCurve)
    {
        UE_LOG(LogG2I, Warning, TEXT("ValveCurve is nullptr in %s"), *GetName());
        return;
    }

    if (ValveTimeline)
    {
        bIsRotating = true;
        bIsValveOpen = false;
        ValveTimeline->ReverseFromEnd();
    }
}

void AG2IPressureGaugeValve::OnTimelineUpdate(float Value)
{
    if (ValveMesh)
    {
        float TargetRoll = Value * 360.0f;

        ValveMesh->SetRelativeRotation(FRotator(TargetRoll, 0.0f, 0.0f));
    }
}

void AG2IPressureGaugeValve::OnTimelineFinished()
{
    bIsRotating = false;

    if (ValveTimeline)
    {
        float CurrentValue = ValveTimeline->GetPlaybackPosition();
    }
    if (ValveCurve)
    {
        float EndValue = ValveCurve->GetFloatValue(ValveTimeline->GetTimelineLength());
    }
}



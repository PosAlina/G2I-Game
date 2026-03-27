#include "Gameplay/G2IPressureGaugeValve.h"
#include "Components/StaticMeshComponent.h"
#include "Curves/CurveFloat.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "G2I.h"
#include "G2IChangingAnglesInterface.h"
#include "G2IWorldHintKeyWidgetComponent.h"

AG2IPressureGaugeValve::AG2IPressureGaugeValve()
{
	PrimaryActorTick.bCanEverTick = true;

	ValveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ValveMesh"));
    if (!ensure(ValveMesh))
    {
        UE_LOG(LogG2I, Warning, TEXT("%s: Couldn't create ValveMesh"), *GetActorNameOrLabel());
    }
    else
    {
        SetRootComponent(ValveMesh);
    }

	ValveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ValveTimeline"));
    if (!ensure(ValveTimeline))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create ValveTimeline"), *GetActorNameOrLabel());
    }

    HintKeyWidgetComp = CreateDefaultSubobject<UG2IWorldHintKeyWidgetComponent>(TEXT("HintKeyWidget"));
    if (!ensure(HintKeyWidgetComp))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create %s"), *GetActorNameOrLabel(),
            *UG2IWorldHintKeyWidgetComponent::StaticClass()->GetName());
    }
    else
    {
        HintKeyWidgetComp->SetupAttachment(RootComponent);
    }
}

void AG2IPressureGaugeValve::Answer_Implementation(const bool bIsRight)
{
    bCanOpen = !bIsRight;
}

void AG2IPressureGaugeValve::Interact_Implementation(const ACharacter* Interactor)
{
    ToggleValve();
    for (AActor *Actor :Puzzle)
    {
        if (Actor && Actor->Implements<UG2IChangingAnglesInterface>())
        {
            IG2IChangingAnglesInterface::Execute_ChangeAngles(Actor, Y, bIsValveOpen);
        }
    }
}

bool AG2IPressureGaugeValve::CanInteract_Implementation(const ACharacter* Interactor)
{
    return !bIsRotating && !bIsLocked;
}

UG2IWorldHintKeyWidgetComponent* AG2IPressureGaugeValve::GetInteractionKeyHintComponent_Implementation()
{
    return HintKeyWidgetComp;
}

void AG2IPressureGaugeValve::SetIsLocked_Implementation(const bool bIsNewLocked)
{
    bIsLocked = bIsNewLocked;

    if (!ensure(HintKeyWidgetComp))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
            *UG2IWorldHintKeyWidgetComponent::StaticClass()->GetName());
        return;
    }
    HintKeyWidgetComp->SetIsLocked_Implementation(bIsLocked);
}

bool AG2IPressureGaugeValve::IsLocked_Implementation()
{
    return bIsLocked;
}

void AG2IPressureGaugeValve::BeginPlay()
{
    Super::BeginPlay();

    bIsAState = false;
    bCanOpen = true;
    bIsRotating = false;

    SetIsLocked_Implementation(bIsLocked);

    if (!ensure(ValveTimeline))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find ValveTimeline"), *GetActorNameOrLabel());
        return;
    }
    
    if (!ensure(ValveCurve))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find ValveCurve"), *GetActorNameOrLabel());
        return;
    }
    
    FOnTimelineFloat UpdateFunction;
    UpdateFunction.BindUFunction(this, FName("OnTimelineUpdate"));
    ValveTimeline->AddInterpFloat(ValveCurve, UpdateFunction);

    FOnTimelineEvent FinishedFunction;
    FinishedFunction.BindUFunction(this, FName("OnTimelineFinished"));
    
    ValveTimeline->SetTimelineFinishedFunc(FinishedFunction);
    ValveTimeline->SetTimelineLength(1.0f);
    if (!ensure(ValveMesh))
    {
        UE_LOG(LogG2I, Warning, TEXT("%s: Couldn't find ValveMesh"), *GetActorNameOrLabel());
        return;
    }
    InitialRotation = ValveMesh->GetRelativeRotation();
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
    if (!ensure(ValveTimeline))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find ValveTimeline"), *GetActorNameOrLabel());
        return;
    }

    bIsRotating = true;
    bIsValveOpen = true;
    ValveTimeline->PlayFromStart();

    G2I::DebugLogMessage(GetActorNameOrLabel() + "Valve is open");
}

void AG2IPressureGaugeValve::CloseValve()
{
    if (!ensure(ValveTimeline))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find ValveTimeline"), *GetActorNameOrLabel());
        return;
    }
    
    bIsRotating = true;
    bIsValveOpen = false;
    ValveTimeline->ReverseFromEnd();

    G2I::DebugLogMessage(GetActorNameOrLabel() + "Valve is close");
}

void AG2IPressureGaugeValve::OnTimelineUpdate(const float Value)
{
    if (!ensure(ValveMesh))
    {
        UE_LOG(LogG2I, Warning, TEXT("%s: Couldn't find ValveMesh"), *GetActorNameOrLabel());
        return;
    }
    
    const float Angle = Value * 360.0f;

    const FQuat InitialQuat = InitialRotation.Quaternion();
    const FQuat RotationQuat = FQuat(FVector::LeftVector, FMath::DegreesToRadians(Angle));

    const FQuat FinalQuat = InitialQuat * RotationQuat;

    ValveMesh->SetRelativeRotation(FinalQuat);
}

void AG2IPressureGaugeValve::OnTimelineFinished()
{
    bIsRotating = false;
}



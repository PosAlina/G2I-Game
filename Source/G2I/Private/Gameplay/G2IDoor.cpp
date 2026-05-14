#include "Gameplay/G2IDoor.h"
#include "Kismet/KismetMathLibrary.h"
#include "G2I.h"
#include "LaunchingIndication/G2ILauncherComponent.h"

AG2IDoor::AG2IDoor()
{
    DoorBaseComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorBase"));
    if (!ensure(DoorBaseComponent))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create Door base Component"), *GetActorNameOrLabel());
    }
    RootComponent = DoorBaseComponent;

    DoorTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DoorTimeline"));
    if (!ensure(DoorTimeline))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create Door Timeline"), *GetActorNameOrLabel());
    }
    LauncherComp = CreateDefaultSubobject<UG2ILauncherComponent>(TEXT("LauncherComp"));
    if (!ensure(LauncherComp))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create %s"), *GetActorNameOrLabel(),
            *UG2ILauncherComponent::StaticClass()->GetName());
    }
}

void AG2IDoor::BeginPlay()
{
    Super::BeginPlay();

    if (!ensure(DoorBaseComponent))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: DoorBaseComponent is NULL"), *GetActorNameOrLabel());
        return;
    }

    StartAngle = DoorBaseComponent->GetRelativeRotation().Yaw;
    DoorBaseComponent->SetMobility(EComponentMobility::Movable);

    if (!ensure(DoorCurve))
    {
        UE_LOG(LogG2I, Warning, TEXT("DoorCurve not assigned in %s"), *GetActorNameOrLabel());
        return;
    }

    FOnTimelineFloat TimelineProgress;
    TimelineProgress.BindUFunction(this, FName("OnTimelineUpdate"));
    DoorTimeline->AddInterpFloat(DoorCurve, TimelineProgress);

    FOnTimelineEvent TimelineFinished;
    TimelineFinished.BindUFunction(this, FName("OnTimelineFinished"));
    DoorTimeline->SetTimelineFinishedFunc(TimelineFinished);

    DoorTimeline->RegisterComponent();
    DoorTimeline->Activate();
    
    LauncherComp->GetOnLockedDelegate().AddDynamic(this, &ThisClass::ToggleLockingDoor);
}

void AG2IDoor::ToggleLockingDoor(UG2ILauncherComponent* LauncherComponent, AActor* ComponentOwner, const bool bIsLocked)
{
    if (bIsLocked && bIsOpen)
    {
        Deactivate_Implementation();
        return;
    }
    if (!bIsLocked && !bIsOpen)
    {
        Activate_Implementation();
        return;
    }
}

void AG2IDoor::RotatingDoor()
{
    if (!ensure(DoorTimeline))
    {
        UE_LOG(LogG2I, Warning, TEXT("(DoorTimeline not assigned in %s"), *GetActorNameOrLabel());
    }

    if (!ensure(DoorCurve))
    {
        UE_LOG(LogG2I, Warning, TEXT("DoorCurve not assigned in %s"), *GetActorNameOrLabel());
    }

    if (!bIsDoorMoving)
    {
        bIsDoorMoving = true;
        bIsOpen = !bIsOpen;

        if (bIsOpen)
            DoorTimeline->Play();
        else
            DoorTimeline->Reverse();
    }
}

void AG2IDoor::OnTimelineUpdate(const float Alpha)
{
    const float TargetYaw = StartAngle + UKismetMathLibrary::Lerp(0.0f, OpenAngle, Alpha);
    const FRotator NewRotation(0.0f, TargetYaw, 0.0f);

    if (!ensure(DoorBaseComponent))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: DoorBaseComponent is NULL"), *GetActorNameOrLabel());
        return;
    }

    DoorBaseComponent->SetRelativeRotation(NewRotation);
    bIsFinished = true;
}

void AG2IDoor::OnTimelineFinished()
{
    bIsDoorMoving = false;
}

void AG2IDoor::Activate_Implementation()
{
    RotatingDoor();
}

void AG2IDoor::Deactivate_Implementation()
{
    RotatingDoor();
}
#include "Gameplay/G2IBaseMovingPlatform.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"
#include "G2I.h"
#include "LaunchingIndication/G2ILauncherComponent.h"

AG2IBaseMovingPlatform::AG2IBaseMovingPlatform()
{
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    if (!ensure(RootComponent))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create RootComponent"), *GetActorNameOrLabel());
        return;
    }

    PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
    if (!ensure(PlatformMesh))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create PlatformMesh"), *GetActorNameOrLabel());
        return;
    }
    PlatformMesh->SetupAttachment(RootComponent);

    StartPosition = CreateDefaultSubobject<UArrowComponent>(TEXT("StartPosition"));
    if (!ensure(StartPosition))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create StartPosition"), *GetActorNameOrLabel());
        return;
    }
    StartPosition->SetupAttachment(RootComponent);

    EndPosition = CreateDefaultSubobject<UArrowComponent>(TEXT("EndPosition"));
    if (!ensure(EndPosition))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create EndPosition"), *GetActorNameOrLabel());
        return;
    }
    EndPosition->SetupAttachment(RootComponent);

    Timeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Timeline"));
    if (!ensure(Timeline))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create Timeline"), *GetActorNameOrLabel());
        return;
    }
    	
    LauncherComp = CreateDefaultSubobject<UG2ILauncherComponent>(TEXT("LauncherComp"));
    if (!ensure(LauncherComp))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create %s"), *GetActorNameOrLabel(),
            *UG2ILauncherComponent::StaticClass()->GetName());
    }
}

void AG2IBaseMovingPlatform::BeginPlay()
{
    Super::BeginPlay();

    if (!ensure(StartPosition) || !ensure(EndPosition))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find StartPosition or EndPosition"), *GetActorNameOrLabel());
        return;
    }

    StartLoc = StartPosition->GetComponentLocation();
    EndLoc = EndPosition->GetComponentLocation();

    ProgressFunction.BindUFunction(this, FName("HandleProgress"));

    if (!ensure(Timeline))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Timeline is NULL"), *GetActorNameOrLabel());
        return;
    }

    if (!ensure(MovementCurve))
    {
        UE_LOG(LogG2I, Error, TEXT("PlatformCurve not assigned in %s"), *GetActorNameOrLabel());
        return;
    }

    Timeline->AddInterpFloat(MovementCurve, ProgressFunction);
    Timeline->SetLooping(false);

    FOnTimelineEvent FinishedEvent;
    FinishedEvent.BindUFunction(this, FName("OnTimelineFinished"));
    Timeline->SetTimelineFinishedFunc(FinishedEvent);
    
    LauncherComp->GetOnLockedDelegate().AddDynamic(this, &ThisClass::ToggleLockingPlatform);
}

void AG2IBaseMovingPlatform::HandleProgress(const float Value)
{
    const FVector NewLocation = FMath::Lerp(StartLoc, EndLoc, Value);
    PlatformMesh->SetWorldLocation(NewLocation);
}

#include "Gameplay/G2IPressureGauge.h"
#include "Components/SceneComponent.h"
#include "G2I.h"
#include "G2IAnswerInterface.h"
#include "LaunchingIndication/G2IActivationLauncherComponent.h"
#include "Sound/G2ISoundComponent.h"

AG2IPressureGauge::AG2IPressureGauge()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;

    LauncherComp = CreateDefaultSubobject<UG2IActivationLauncherComponent>(TEXT("LauncherComp"));
    if (!ensure(LauncherComp))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
            *UG2IActivationLauncherComponent::StaticClass()->GetName());
    }
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
    SoundComp = CreateDefaultSubobject<UG2ISoundComponent>(TEXT("GaugeSoundComponent"));
    if (SoundComp) {
        SoundComp->SetupAttachment(RootComponent);
        SoundComp->SetupSounds.Add(TEXT("ArrowRotationSound"), FSoundConfig());
    }
}

void AG2IPressureGauge::BeginPlay()
{
    Super::BeginPlay();

    SetupDefaults();
    BindDelegates();

    if (SoundComp && SoundComp->SetupSounds.Contains(TEXT("ArrowRotationSound"))) {
        ArrowRotationSoundId = SoundComp->AddSound(SoundComp->SetupSounds[TEXT("ArrowRotationSound")]);
        if (ArrowRotationSoundId == -1)
        {
            UE_LOG(LogG2I, Warning, TEXT("[%s][%s]: ArrowRotationSoundId (ID == -1)"), *GetName(), *FString(__FUNCTION__));
        }
    }
}

void AG2IPressureGauge::SetupDefaults()
{
    if (!ensure(LauncherComp))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
            *UG2ILauncherComponent::StaticClass()->GetName());
        return;
    }
    SetIsLockedPuzzleActors(LauncherComp->IsLocked_Implementation());
}

void AG2IPressureGauge::BindDelegates()
{
    if (!ensure(LauncherComp))
    {
        UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
            *UG2ILauncherComponent::StaticClass()->GetName());
    }
    LauncherComp->GetOnLockedDelegate().AddDynamic(this, &ThisClass::LockedPuzzleActors);
}

void AG2IPressureGauge::SetIsLockedPuzzleActors(const bool bIsNewLocked)
{
    for (AActor *Actor : PuzzleActors)
    {
        if (Actor && Actor->Implements<UG2ILockingInterface>())
        {
            IG2ILockingInterface::Execute_SetIsLocked(Actor, bIsNewLocked);
        }
    }
    FString DebugMessage = GetActorNameOrLabel() + " ";
    DebugMessage += bIsNewLocked ? TEXT("locked") : TEXT("un locked");
    G2I::DebugLogMessage(DebugMessage);
}

void AG2IPressureGauge::LockedPuzzleActors(UG2ILauncherComponent* LauncherComponent, AActor* ComponentOwner,
                                           const bool bIsLocked)
{
    SetIsLockedPuzzleActors(bIsLocked);
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

void AG2IPressureGauge::ChangeAngles_Implementation(const TArray<float>& AngleDeltas, const bool bIsOn)
{
    ArrowsAngle(bIsOn, AngleDeltas);

    const bool bIsCorrect = CheckTargetAngles();
    for (AActor *Actor : PuzzleActors)
    {
        if (Actor && Actor->Implements<UG2IAnswerInterface>())
        {
            IG2IAnswerInterface::Execute_Answer(Actor, bIsCorrect);
        }
    }
    if (bIsCorrect)
    {
        G2I::DebugLogMessage(GetActorNameOrLabel() + "Pressure Gauge completed successfully");
        
        if (!ensure(LauncherComp))
        {
            UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
                *UG2ILauncherComponent::StaticClass()->GetName());
        }
        else
        {
            LauncherComp->CallActivation();
        }
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

        const float CurrentPitch = FMath::Lerp(Movement.StartPitch, Movement.TargetPitch, Alpha);

        const float NormalizedPitch = FRotator::NormalizeAxis(CurrentPitch);
        Comp->SetRelativeRotation(FRotator(NormalizedPitch, 0.0f, 0.0f));

        if (Alpha >= 1.0f)
        {
            ActiveMovements.RemoveAt(i);
        }
    }

    if (ActiveMovements.Num() == 0)
    {
        if (SoundComp) {
            SoundComp->StopSound(ArrowRotationSoundId);
        }
        SetActorTickEnabled(false);
    }
}

void AG2IPressureGauge::SmoothMoveComponent(USceneComponent* Component, const float StartPitch, const float TargetPitch,
    const float MoveTime)
{
    if (!Component)
    {
        UE_LOG(LogG2I, Error, TEXT("Gauge '%s': SmoothMoveComponent with null component"), *GetName());
        return;
    }

    if (MoveTime <= 0.0f)
    {
        const float NormalizedPitch = FRotator::NormalizeAxis(TargetPitch);
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

    if (SoundComp) {
        SoundComp->PlaySound(ArrowRotationSoundId);
    }

    SetActorTickEnabled(true);
}

void AG2IPressureGauge::ArrowsAngle(const bool bIsOn, const TArray<float>& AngleDeltas)
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

bool AG2IPressureGauge::CheckTargetAngles(const float Tolerance) const
{
    for (const FArrowInfo& Info : Arrows)
    {
        if (!Info.ArrowComponent)
            return false;

        float Current = FMath::Fmod(Info.CurrentPitch, 360.0f);
        if (Current < 0.0f)
            Current += 360.0f;

        const float Target = Info.TargetPitch;

        float Diff = FMath::Abs(Current - Target);
        if (Diff > 180.0f)
            Diff = 360.0f - Diff;

        if (!FMath::IsNearlyZero(Diff, Tolerance))
            return false;
    }

    return true;
}
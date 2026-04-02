#include "Gameplay/G2IPortal.h"
#include "GameFramework/Character.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "G2I.h"
#include "G2IPlayerController.h"


// Sets default values
AG2IPortal::AG2IPortal()
{
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
    Arrow->SetupAttachment(RootComponent);

    CameraRotationArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("CameraRotationArrow"));
    CameraRotationArrow->SetupAttachment(RootComponent);

    PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMesh"));
    PortalMesh->SetupAttachment(RootComponent);
    PortalMesh->SetCollisionProfileName(TEXT("BlockAll"));

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    TriggerBox->SetupAttachment(RootComponent);
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AG2IPortal::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (CameraRotationArrow)
    {
        CameraRotationArrow->SetArrowFColor(FColor::Blue);
        CameraRotationArrow->SetRelativeLocation(FVector(30.f, 0.f, 50.f));
        CameraRotationArrow->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
    }

    if (TriggerBox)
    {
        TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
        TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
        TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
        // Also enabling overlaps for passing through actor
        TriggerBox->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap);
        TriggerBox->SetBoxExtent(FVector(5.f, 60.f, 125.f));
    }
}

void AG2IPortal::BeginPlay()
{
    Super::BeginPlay();

    if (TriggerBox)
    {
        TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AG2IPortal::OverlapTrigger);
    }

    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        PlayerController = Cast<AG2IPlayerController>(PC);
    }

    if (!PlayerController)
    {
        UE_LOG(LogG2I, Warning, TEXT("PlayerController not found %s"), *GetName());
    }
}

void AG2IPortal::OverlapTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor)
    {
        UE_LOG(LogG2I, Warning, TEXT("Overlapped Actor is null in %s"), *GetActorNameOrLabel());
        return;
    }

    ACharacter* Character = Cast<ACharacter>(OtherActor);

    if (!Character)
    {
        UE_LOG(LogG2I, Warning, TEXT("Overlapped Actor isn't character in %s"), *GetActorNameOrLabel());
        return;
    }

    Teleport(Character);
}

void AG2IPortal::Teleport(ACharacter* Interactor) const
{
    if (Interactor)
    {
        if (TargetPortal && bIsActive && TargetPortal->bIsActive && TargetPortal->Arrow)
        {
            const FVector TargetLocation = TargetPortal->Arrow->GetComponentLocation();
            const FRotator TargetRotation = TargetPortal->Arrow->GetComponentRotation();
            const FRotator TargetCameraRotation = TargetPortal->CameraRotationArrow->GetComponentRotation();

            Interactor->SetActorLocationAndRotation(TargetLocation, TargetRotation);

            if (Interactor->IsPlayerControlled() && PlayerController)
            {
                PlayerController->RotateCameraTo(TargetCameraRotation.Yaw, TargetCameraRotation.Pitch);
            }
        }
    }
    else
    {
        UE_LOG(LogG2I, Warning, TEXT("Interactor is null in %s"), *GetActorNameOrLabel());
    }
}

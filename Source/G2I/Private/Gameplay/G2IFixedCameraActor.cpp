#include "G2IFixedCameraActor.h"
#include "G2I.h"
#include "Components/BoxComponent.h"
#include "Components/G2ICharacterCollisionComponent.h"
#include "Kismet/KismetMathLibrary.h"

AG2IFixedCameraActor::AG2IFixedCameraActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(FName("Root")));
	InitializeFixedCameraZoneTrigger();
	InitializeFixedCamera();
}

void AG2IFixedCameraActor::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World is null %s"), *GetName());
	}

	PlayerController = GetWorld()->GetFirstPlayerController();
}

UBoxComponent *AG2IFixedCameraActor::GetFixedCameraZoneTrigger()
{
	return FixedCameraZoneTrigger;
}

UCameraComponent* AG2IFixedCameraActor::GetFixedCamera()
{
	return FixedCamera;
}

void AG2IFixedCameraActor::InitializeFixedCameraZoneTrigger()
{
	FixedCameraZoneTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger zone"));
	if (!ensure(FixedCameraZoneTrigger))
	{
		UE_LOG(LogG2I, Error, TEXT("There is not any camera in all camera components %s"), *GetName());
		return;
	}
	
	FixedCameraZoneTrigger->SetupAttachment(RootComponent);
	FixedCameraZoneTrigger->SetRelativeScale3D(FVector(10.0f, 10.0f, 10.0f));
	FixedCameraZoneTrigger->SetCollisionResponseToChannel(
		UG2ICharacterCollisionComponent::GetPassingThroughCollisionChannel(), ECR_Overlap);
}

void AG2IFixedCameraActor::InitializeFixedCamera()
{
	FixedCamera = CreateDefaultSubobject<UCineCameraComponent>(TEXT("Fixed camera"));
	if (!ensure(FixedCamera))
	{
		UE_LOG(LogG2I, Error, TEXT("There is not any camera in all camera components %s"), *GetName());
		return;
	}
	
	FixedCamera->SetupAttachment(RootComponent);
	FixedCamera->SetRelativeLocation(FVector(-900.0f, 0.0f, 400.0f));
	FixedCamera->SetRelativeRotation(FRotator(-30, 0, 0));
}

void AG2IFixedCameraActor::StartFollow()
{
	if (!bShouldFollow)
	{
		return;
	}

	SetActorTickEnabled(true);
}

void AG2IFixedCameraActor::StopFollow()
{
	SetActorTickEnabled(false);
}

void AG2IFixedCameraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("PlayerController is nullptr %s"), *GetName());
		return;
	}

	if (!ensure(FixedCamera))
	{
		UE_LOG(LogG2I, Error, TEXT("FixedCamera is nullptr %s"), *GetName());
		return;
	}

	const FVector CameraLocation = FixedCamera->GetComponentLocation();
	const FVector TargetLocation = PlayerController->GetPawn()->GetActorLocation();

	const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(CameraLocation, TargetLocation);
	const FRotator SmoothRotation = FMath::RInterpTo(FixedCamera->GetComponentRotation(), LookAtRotation, DeltaTime, 3.0f);

	FixedCamera->SetWorldRotation(SmoothRotation);
}
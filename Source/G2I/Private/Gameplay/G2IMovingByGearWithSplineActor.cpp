#include "Gameplay/G2IMovingByGearWithSplineActor.h"
#include "G2I.h"
#include "Components/SplineComponent.h"
#include "Components/BoxComponent.h"

void AG2IMovingByGearWithSplineActor::SetLocationAndRotationWithSpline(float SplineDistance)
{
	if (!SplineComponent || !MainBoxComponent) return;

	FVector NewLocation = SplineComponent->GetLocationAtDistanceAlongSpline(
		SplineDistance,
		ESplineCoordinateSpace::World
	);

	FRotator NewRotation = SplineComponent->GetRotationAtDistanceAlongSpline(
		SplineDistance,
		ESplineCoordinateSpace::World
	);

	FHitResult Hit;
	MainBoxComponent->SetWorldLocationAndRotation(NewLocation, NewRotation, true, &Hit);

	if (Hit.bBlockingHit)
	{
		CurrentSplineDistance = SplineComponent->GetDistanceAlongSplineAtLocation(
			MainBoxComponent->GetComponentLocation(),
			ESplineCoordinateSpace::World);
	}
}


AG2IMovingByGearWithSplineActor::AG2IMovingByGearWithSplineActor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineToMoveComponent"));
	if (!SplineComponent) {
		UE_LOG(LogG2I, Warning, TEXT("SplineComponent was not created for %s"), *GetName());
		return;
	}
	SplineComponent->SetupAttachment(RootComponent);

	MainBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("MainBoxComponent"));
	if (!MainBoxComponent) {
		UE_LOG(LogG2I, Warning, TEXT("MainBoxComponent was not created for %s"), *GetName());
		return;
	}
	MainBoxComponent->SetupAttachment(SplineComponent);
}

void AG2IMovingByGearWithSplineActor::BeginPlay()
{
	Super::BeginPlay();

	if (!SplineComponent) {
		UE_LOG(LogG2I, Warning, TEXT("SplineComponent is not set for %s"), *GetName());
		return;
	}

	SetLocationAndRotationWithSpline(0.0f);
}

void AG2IMovingByGearWithSplineActor::OnPushing_Implementation(float ForceMagnitude)
{
	if (!SplineComponent) {
		UE_LOG(LogG2I, Warning, TEXT("SplineComponent is not set for %s"), *GetName());
		return;
	}
	float SplineLength = SplineComponent->GetSplineLength();
	CurrentSplineDistance += ForceMagnitude * ForceModifier * SplineLength;
	CurrentSplineDistance = FMath::Clamp(CurrentSplineDistance, 0.0f, SplineLength);

	SetLocationAndRotationWithSpline(CurrentSplineDistance);
}
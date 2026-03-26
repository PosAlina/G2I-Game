#include "Gameplay/G2IMovingByGearWithSplineActor.h"
#include "G2I.h"
#include "Components/SplineComponent.h"
#include "Components/BoxComponent.h"

void AG2IMovingByGearWithSplineActor::SetLocationAndRotationWithSpline(float SplineDistance)
{
	if (!ensure(SplineComponent)) {
		UE_LOG(LogG2I, Warning, TEXT("Couldn't get the Spline Component for %s"), *GetName());
		return;
	}

	if (!ensure(MainBoxComponent)) {
		UE_LOG(LogG2I, Warning, TEXT("Couldn't get the Main Box Component for %s"), *GetName());
		return;
	}

	FVector NewLocation;
	if (bChangeLocationThroughSpline) {
		NewLocation = SplineComponent->GetLocationAtDistanceAlongSpline(
			SplineDistance,
			ESplineCoordinateSpace::World
		);
	} 
	else {
		NewLocation = MainBoxComponent->GetComponentLocation();
	}

	FRotator NewRotation;
	if (bChangeRotationThroughSpline) {
		float InputKey = SplineComponent->GetInputKeyAtDistanceAlongSpline(SplineDistance);

		int32 CurrentPointIndex = FMath::TruncToInt(InputKey);
		int32 NextPointIndex = FMath::Clamp(CurrentPointIndex + 1, 0, SplineComponent->GetNumberOfSplinePoints() - 1);

		float Alpha = InputKey - CurrentPointIndex;
		FRotator StartRot = SplineComponent->GetRotationAtSplinePoint(CurrentPointIndex, ESplineCoordinateSpace::World);
		FRotator EndRot = SplineComponent->GetRotationAtSplinePoint(NextPointIndex, ESplineCoordinateSpace::World);

		NewRotation = FMath::Lerp(StartRot, EndRot, Alpha);
	} 
	else {
		NewRotation = MainBoxComponent->GetComponentRotation();
	}

	FHitResult Hit;
	MainBoxComponent->SetWorldLocationAndRotation(NewLocation, NewRotation, bCheckHit, &Hit);

	if (bCheckHit && Hit.bBlockingHit)
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
	if (!ensure(SplineComponent)) {
		UE_LOG(LogG2I, Warning, TEXT("SplineComponent was not created for %s"), *GetName());
		return;
	}
	SplineComponent->SetupAttachment(RootComponent);

	MainBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("MainBoxComponent"));
	if (!ensure(MainBoxComponent)) {
		UE_LOG(LogG2I, Warning, TEXT("MainBoxComponent was not created for %s"), *GetName());
		return;
	}
	MainBoxComponent->SetupAttachment(SplineComponent);
}

void AG2IMovingByGearWithSplineActor::BeginPlay()
{
	Super::BeginPlay();

	if (!ensure(SplineComponent)) {
		UE_LOG(LogG2I, Warning, TEXT("SplineComponent is not set for %s"), *GetName());
		return;
	}

	SetLocationAndRotationWithSpline(0.0f);
}

void AG2IMovingByGearWithSplineActor::OnPushing_Implementation(float ForceMagnitude)
{
	if (!ensure(SplineComponent)) {
		UE_LOG(LogG2I, Warning, TEXT("SplineComponent is not set for %s"), *GetName());
		return;
	}
	float SplineLength = SplineComponent->GetSplineLength();
	CurrentSplineDistance += ForceMagnitude * ForceModifier * SplineLength;
	CurrentSplineDistance = FMath::Clamp(CurrentSplineDistance, 0.0f, SplineLength);

	SetLocationAndRotationWithSpline(CurrentSplineDistance);
}
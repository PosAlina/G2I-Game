#include "Gameplay/G2IMovingByGearWithSplineActor.h"
#include "G2I.h"
#include "Components/SplineComponent.h"
#include "Components/BoxComponent.h"

bool AG2IMovingByGearWithSplineActor::SetLocationAndRotationWithSpline(float SplineDistance)
{
	if (!ensure(SplineComponent)) {
		UE_LOG(LogG2I, Warning, TEXT("Couldn't get the Spline Component for %s"), *GetName());
		return true;
	}

	if (!ensure(MainBoxComponent)) {
		UE_LOG(LogG2I, Warning, TEXT("Couldn't get the Main Box Component for %s"), *GetName());
		return true;
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
	if (ChangeRotationThroughSpline == EG2IRotationType::PointRotation) {
		const float InputKey = SplineComponent->GetInputKeyAtDistanceAlongSpline(SplineDistance);

		const int32 CurrentPointIndex = FMath::TruncToInt(InputKey);
		const int32 NextPointIndex = FMath::Clamp(CurrentPointIndex + 1, 0, SplineComponent->GetNumberOfSplinePoints() - 1);

		const float Alpha = InputKey - CurrentPointIndex;
		const FRotator StartRot = SplineComponent->GetRotationAtSplinePoint(CurrentPointIndex, ESplineCoordinateSpace::World);
		const FRotator EndRot = SplineComponent->GetRotationAtSplinePoint(NextPointIndex, ESplineCoordinateSpace::World);

		NewRotation = FMath::Lerp(StartRot, EndRot, Alpha);
	} 
	else if (ChangeRotationThroughSpline == EG2IRotationType::SplineRotation) {
		NewRotation = SplineComponent->GetRotationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World);
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
		return true;
	}
	if (FMath::IsNearlyEqual(SplineDistance, SplineComponent->GetSplineLength())) {
		return true;
	}
	if (FMath::IsNearlyEqual(SplineDistance, 0.f)) {
		return true;
	}

	return false;
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

bool AG2IMovingByGearWithSplineActor::OnPushing_Implementation(float ForceMagnitude)
{
	if (!ensure(SplineComponent)) {
		UE_LOG(LogG2I, Warning, TEXT("SplineComponent is not set for %s"), *GetName());
		return true;
	}
	const float SplineLength = SplineComponent->GetSplineLength();
	CurrentSplineDistance += ForceMagnitude * ForceModifier;
	CurrentSplineDistance = FMath::Clamp(CurrentSplineDistance, 0.0f, SplineLength);

	return SetLocationAndRotationWithSpline(CurrentSplineDistance);
}
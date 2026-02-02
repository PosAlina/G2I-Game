#include "Gameplay/G2IMovingByGearWithSplineActor.h"
#include "G2I.h"
#include "Components/SplineComponent.h"

void AG2IMovingByGearWithSplineActor::SetLocationAndRotationWithSpline(float SplineDistance)
{
	FVector NewLocation = \
		SplineComponent->GetLocationAtDistanceAlongSpline(
			SplineDistance,
			ESplineCoordinateSpace::World
		);

	FRotator NewRotation = \
		SplineComponent->GetRotationAtDistanceAlongSpline(
			SplineDistance,
			ESplineCoordinateSpace::World
		);

	TArray<USceneComponent*> ChildComponents;
	SplineComponent->GetChildrenComponents(true, ChildComponents);
	
	for (USceneComponent* Child : ChildComponents)
	{
		if (UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(Child))
		{
			MeshComp->SetWorldLocationAndRotation(NewLocation, NewRotation);
		}
	}
}

AG2IMovingByGearWithSplineActor::AG2IMovingByGearWithSplineActor()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineToMoveComponent"));
	SplineComponent->SetupAttachment(RootComponent);
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


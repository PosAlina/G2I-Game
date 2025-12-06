
#include "Gameplay/G2IPipe.h"
#include "Components/ArrowComponent.h"

// Sets default values
AG2IPipe::AG2IPipe()
{
	SplineMetadata = CreateDefaultSubobject<UG2IPipesSplineMetadata>(TEXT("SplineMetadata"));

	SplineComponent = CreateDefaultSubobject<UG2IPipesSplineComponent>(TEXT("SplineComponent"));
	if (SplineComponent)
		SetRootComponent(SplineComponent);
}

void AG2IPipe::OnConstruction(const FTransform& Transform)
{
	if (!DefaultMesh) return;

	UStaticMesh* Mesh;

	for (int PointCount = 0; PointCount < (SplineComponent->GetNumberOfSplinePoints() - 1); PointCount++)
	{
		// Setting Up Static Mesh
		if (GetHasPipeAtSplinePoint(PointCount))
			Mesh = DefaultMesh;
		else
			continue;

		// Generating Mesh
		USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
		SplineMesh->SetStaticMesh(Mesh);
		SplineMesh->SetMobility(EComponentMobility::Stationary);
		SplineMesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		SplineMesh->RegisterComponentWithWorld(GetWorld());
		SplineMesh->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);

		// Applying Mesh Transform
		FVector StartPoint, EndPoint, StartTangent, EndTangent;
		SplineComponent->GetLocationAndTangentAtSplinePoint(PointCount, StartPoint, StartTangent, ESplineCoordinateSpace::Local);
		SplineComponent->GetLocationAndTangentAtSplinePoint(PointCount + 1, EndPoint, EndTangent, ESplineCoordinateSpace::Local);

		SplineMesh->SetStartAndEnd(StartPoint, StartTangent, EndPoint, EndTangent, true);
		SplineMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		SplineMesh->SetForwardAxis(ForwardAxis);
		SplineMesh->SetSplineUpDir(SplineComponent->GetUpVectorAtSplinePoint(PointCount, ESplineCoordinateSpace::Local));

		// Debug Arrows
#if WITH_EDITOR
		if (bShowDebugUpArrows)
		{
			UArrowComponent* Arrow = (UArrowComponent*)(AddComponentByClass(UArrowComponent::StaticClass(), false, SplineComponent->GetTransformAtSplinePoint(PointCount, ESplineCoordinateSpace::Local), false));
			Arrow->SetRelativeTransform(SplineComponent->GetTransformAtSplinePoint(PointCount, ESplineCoordinateSpace::Local));
			Arrow->SetRelativeLocationAndRotation(SplineComponent->GetLocationAtSplinePoint(PointCount, ESplineCoordinateSpace::Local),
				SplineMesh->GetSplineUpDir().Rotation());
		}
#endif // Debug Arrows
	}
}

TObjectPtr<UG2IPipesSplineMetadata> AG2IPipe::GetSplineMetadata() const
{
	return SplineMetadata;
}

float AG2IPipe::GetTestFloatAtSplinePoint(int32 PointIndex)
{
	if (ensure(SplineMetadata))
	{
		if (ensure(SplineMetadata->PointParams.IsValidIndex(PointIndex)))
		{
			return SplineMetadata->PointParams[PointIndex].TestValue;
		}
	}

	return 0.0f;
}

bool AG2IPipe::GetHasPipeAtSplinePoint(int32 PointIndex)
{
	if (ensure(SplineMetadata))
	{
		if (ensure(SplineMetadata->PointParams.IsValidIndex(PointIndex)))
		{
			return SplineMetadata->PointParams[PointIndex].bHasPipe;
		}
	}

	return false;
}


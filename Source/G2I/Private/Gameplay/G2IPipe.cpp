
#include "Gameplay/G2IPipe.h"

// Sets default values
AG2IPipe::AG2IPipe()
{
	SplineMetadata = CreateDefaultSubobject<UG2IPipesSplineMetadata>(TEXT("SplineMetadata"));

	SplineComponent = CreateDefaultSubobject<UG2IPipesSplineComponent>(TEXT("SplineComponent"));
	if (SplineComponent)
		SetRootComponent(SplineComponent);
}

TObjectPtr<UG2IPipesSplineMetadata> AG2IPipe::GetSplineMetadata() const
{
	return SplineMetadata;
}


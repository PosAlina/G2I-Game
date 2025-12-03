#include "Components/G2IPipesSplineComponent.h"
#include "Gameplay/G2IPipe.h"

USplineMetadata* UG2IPipesSplineComponent::GetSplinePointsMetadata()
{
	if (AG2IPipe* Owner = Cast<AG2IPipe>(GetOwner()))
	{
		return Owner->GetSplineMetadata();
	}

	return nullptr;
}

const USplineMetadata* UG2IPipesSplineComponent::GetSplinePointsMetadata() const
{
	if (AG2IPipe* Owner = Cast<AG2IPipe>(GetOwner()))
	{
		return Owner->GetSplineMetadata();
	}

	return nullptr;
}

void UG2IPipesSplineComponent::PostLoad()
{
	Super::PostLoad();

	FixupPoints();
}

void UG2IPipesSplineComponent::PostDuplicate(bool bDuplicateForPie)
{
	Super::PostDuplicate(bDuplicateForPie);

	FixupPoints();
}

void UG2IPipesSplineComponent::FixupPoints()
{
#if WITH_EDITORONLY_DATA
	// Keep metadata in sync
	if (GetSplinePointsMetadata())
	{
		const int32 NumPoints = GetNumberOfSplinePoints();
		GetSplinePointsMetadata()->Fixup(NumPoints, this);
	}
#endif
}

#if WITH_EDITOR
void UG2IPipesSplineComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FixupPoints();
}

void UG2IPipesSplineComponent::PostEditImport()
{
	Super::PostEditImport();

	FixupPoints();
}
#endif
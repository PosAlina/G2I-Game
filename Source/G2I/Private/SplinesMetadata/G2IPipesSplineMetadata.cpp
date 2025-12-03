#include "SplinesMetadata/G2IPipesSplineMetadata.h"

void UG2IPipesSplineMetadata::InsertPoint(int32 Index, float t, bool bClosedLoop)
{
	if (Index >= PointParams.Num())
	{
		AddPoint(static_cast<float>(Index));
	}
	else
	{
		PointParams.Insert(FG2IPipesSplinePointParams{}, Index);
	}

	Modify();
}

void UG2IPipesSplineMetadata::UpdatePoint(int32 Index, float t, bool bClosedLoop)
{
	Modify();
}

void UG2IPipesSplineMetadata::AddPoint(float InputKey)
{
	PointParams.Add(FG2IPipesSplinePointParams{});
	Modify();
}

void UG2IPipesSplineMetadata::RemovePoint(int32 Index)
{
	PointParams.RemoveAt(Index);
	Modify();
}

void UG2IPipesSplineMetadata::DuplicatePoint(int32 Index)
{
	FG2IPipesSplinePointParams NewVal = PointParams[Index];
	PointParams.Insert(NewVal, Index);
	Modify();
}

void UG2IPipesSplineMetadata::CopyPoint(const USplineMetadata* FromSplineMetadata, int32 FromIndex, int32 ToIndex)
{
	if (const UG2IPipesSplineMetadata* FromMetadata = Cast<UG2IPipesSplineMetadata>(FromSplineMetadata))
	{
		PointParams[ToIndex] = PointParams[FromIndex];
		Modify();
	}
}

void UG2IPipesSplineMetadata::Reset(int32 NumPoints)
{
	PointParams.Reset(NumPoints);
	Modify();
}

void UG2IPipesSplineMetadata::Fixup(int32 NumPoints, USplineComponent* SplineComp)
{
	if (PointParams.Num() > NumPoints)
	{
		PointParams.RemoveAt(NumPoints, PointParams.Num() - NumPoints);
		Modify();
	}

	while (PointParams.Num() < NumPoints)
	{
		PointParams.Add(FG2IPipesSplinePointParams{});
		Modify();
	}
}

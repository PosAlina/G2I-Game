#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include <Components/G2IPipesSplineComponent.h>
#include "G2IPipesSplineMetadata.generated.h"

UCLASS()
class G2I_API UG2IPipesSplineMetadata : public USplineMetadata
{
	GENERATED_BODY()

public:
	virtual void InsertPoint(int32 Index, float t, bool bClosedLoop) override;
	virtual void UpdatePoint(int32 Index, float t, bool bClosedLoop) override;
	virtual void AddPoint(float InputKey) override;
	virtual void RemovePoint(int32 Index) override;
	virtual void DuplicatePoint(int32 Index) override;
	virtual void CopyPoint(const USplineMetadata* FromSplineMetadata, int32 FromIndex, int32 ToIndex) override;
	virtual void Reset(int32 NumPoints) override;
	virtual void Fixup(int32 NumPoints, USplineComponent* SplineComp) override;

	UPROPERTY(EditAnywhere)
	TArray<FG2IPipesSplinePointParams> PointParams;
};

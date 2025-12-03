#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "G2IPipesSplineComponent.generated.h"


/**
Struct that holds custom data per spline point.
 */
USTRUCT()
struct FG2IPipesSplinePointParams
{
	GENERATED_BODY()

public:

	// Properties for generating meshes

	UPROPERTY(EditAnywhere)
	float TestValue = 0.0f;

	UPROPERTY(EditAnywhere)
	bool bHasPipe = true;

	UPROPERTY(EditAnywhere)
	bool bHasValve = true;

	UPROPERTY(EditAnywhere)
	bool bHasTechnicalHole = false;

	// Properties for connecting pipes

	UPROPERTY(EditAnywhere)
	bool bSendToOtherPipe = false;

	UPROPERTY(EditAnywhere)
	bool bResieveFromOtherPipe = false;

};

/**
Spline component for PIPES that uses custom point parameters.
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class G2I_API UG2IPipesSplineComponent : public USplineComponent
{
	GENERATED_BODY()

public:
	virtual USplineMetadata* GetSplinePointsMetadata() override;
	virtual const USplineMetadata* GetSplinePointsMetadata() const override;
	virtual void PostLoad() override;
	virtual void PostDuplicate(bool bDuplicateForPie) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditImport() override;
#endif

	void FixupPoints();
};

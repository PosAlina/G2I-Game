#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineMeshComponent.h"
#include "Components/G2IPipesSplineComponent.h"
#include "SplinesMetadata/G2IPipesSplineMetadata.h"
#include "G2IPipe.generated.h"

UCLASS()
class G2I_API AG2IPipe : public AActor
{
	GENERATED_BODY()
	
public:
	AG2IPipe();

	void OnConstruction(const FTransform& Transform) override;

	TObjectPtr<UG2IPipesSplineMetadata> GetSplineMetadata() const;

	UFUNCTION(BlueprintCallable)
	float GetTestFloatAtSplinePoint(int32 PointIndex);

	UFUNCTION(BlueprintCallable)
	bool GetHasPipeAtSplinePoint(int32 PointIndex);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spline")
	TObjectPtr<UStaticMesh> DefaultMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
	TEnumAsByte<ESplineMeshAxis::Type> ForwardAxis;

private:
	UPROPERTY(Instanced, Export)
	TObjectPtr<UG2IPipesSplineMetadata> SplineMetadata;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UG2IPipesSplineComponent> SplineComponent;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Spline")
	bool bShowDebugUpArrows = false;
#endif
};

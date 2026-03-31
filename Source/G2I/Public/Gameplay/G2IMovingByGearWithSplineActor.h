#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/G2IMovingByGearObjectInterface.h"
#include "G2IMovingByGearWithSplineActor.generated.h"

class USplineComponent;
class UBoxComponent;
UCLASS()
class G2I_API AG2IMovingByGearWithSplineActor : public AActor, public IG2IMovingByGearObjectInterface
{
	GENERATED_BODY()

private:
	void SetLocationAndRotationWithSpline(float SplineDistance);
public:
	UPROPERTY(EditAnywhere, Category = "Data|Spline")
	TObjectPtr<USplineComponent> SplineComponent;

	UPROPERTY(EditAnywhere, Category = "Data|Spline")
	TObjectPtr<UBoxComponent> MainBoxComponent;

	UPROPERTY(EditAnywhere, Category = "Data|Spline")
	float CurrentSplineDistance;

	UPROPERTY(EditAnywhere, Category = "Data|Spline")
	bool bChangeRotationThroughSpline = true;

	UPROPERTY(EditAnywhere, Category = "Data|Spline")
	bool bChangeLocationThroughSpline = true;

	UPROPERTY(EditAnywhere, Category = "Data|Spline")
	bool bCheckHit = true;

	AG2IMovingByGearWithSplineActor();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Data")
	float ForceModifier = 1.0f;

	virtual void OnPushing_Implementation(float ForceMagnitude) override;
};

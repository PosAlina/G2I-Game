#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/G2IPipesSplineComponent.h"
#include "SplinesMetadata/G2IPipesSplineMetadata.h"
#include "G2IPipe.generated.h"

UCLASS()
class G2I_API AG2IPipe : public AActor
{
	GENERATED_BODY()
	
public:
	AG2IPipe();

	TObjectPtr<UG2IPipesSplineMetadata> GetSplineMetadata() const;

private:
	UPROPERTY(Instanced, Export)
	TObjectPtr<UG2IPipesSplineMetadata> SplineMetadata;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UG2IPipesSplineComponent> SplineComponent;
};

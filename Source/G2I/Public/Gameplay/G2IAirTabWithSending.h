#pragma once

#include "CoreMinimal.h"
#include "G2IAirTab.h"
#include "G2ISendToPipesComponent.h"
#include "G2IAirTabWithSending.generated.h"

/**
 * Air Tab that also sends air
 */
UCLASS(Blueprintable, Placeable)
class G2I_API AG2IAirTabWithSending : public AG2IAirTab
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UG2ISendToPipesComponent> SendingToPipesComponent;

public:
	AG2IAirTabWithSending();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void ChangeActivated(const bool bNewActivated) override;

protected:
	virtual void BeginPlay() override;
};
 
#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "G2IPipesBoxComponent.generated.h"

/**
 * Allows to send air or recieve it
 */
UCLASS()
class G2I_API UG2IPipesBoxComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	bool bRecieves = true;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AActor> Owner;

	UPROPERTY(VisibleAnywhere)
	int32 PointIndex;

public:
	UG2IPipesBoxComponent();

	virtual void InitializeComponent() override;
};

#pragma once

#include "CoreMinimal.h"
#include "Components/G2IActivationWithOrderComponent.h"
#include "Interfaces/G2IActivationInterface.h"
#include "G2IActivationOrderSMeshComponent.generated.h"

/**
 * ActivationWithOrder component that on accepted/declined
 * calls activation functions on it's owner
 * and ignores ActivatedIndexes array from it's parent.
 */
UCLASS()
class G2I_API UG2IActivationOrderSMeshComponent : public UG2IActivationWithOrderComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Activation with order")
	bool bAccepted = false;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<AActor> OwnerActor;

public:
	virtual void BeginPlay() override;

	virtual void Accepted(int32 AtIndex) override;

	virtual void Declined() override;

	virtual void Restored() override;
};

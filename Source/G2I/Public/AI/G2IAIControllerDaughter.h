#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "G2IAIControllerDaughter.generated.h"

class UG2IAIMovementComponent;

/**
 * AI Controller of daughter
 */
UCLASS()
class G2I_API AG2IAIControllerDaughter : public AAIController
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UG2IAIMovementComponent> MovementComponent;

public:

	AG2IAIControllerDaughter();

protected:

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;
	
};
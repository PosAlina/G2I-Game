#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "G2IAIControllerEngineer.generated.h"

class UG2IAIMovementComponent;

/**
 * AI Controller of engineer
 */
UCLASS()
class G2I_API AG2IAIControllerEngineer : public AAIController
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UG2IAIMovementComponent> MovementComponent;

public:
	
	AG2IAIControllerEngineer();

protected:
	
	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;
	
};

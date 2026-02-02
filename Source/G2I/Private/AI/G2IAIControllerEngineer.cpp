#include "G2IAIControllerEngineer.h"
#include "G2IAIMovementComponent.h"

AG2IAIControllerEngineer::AG2IAIControllerEngineer()
{
	MovementComponent = CreateDefaultSubobject<UG2IAIMovementComponent>(TEXT("MovementComp"));
}

void AG2IAIControllerEngineer::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (MovementComponent)
	{
		MovementComponent->UpdateNewAIPawn(InPawn);

		if (MovementComponent->Implements<UG2IActivationInterface>())
		{
			IG2IActivationInterface::Execute_Activate(MovementComponent);
		}
	}
}

void AG2IAIControllerEngineer::OnUnPossess()
{
	Super::OnUnPossess();

	if (MovementComponent && MovementComponent->Implements<UG2IActivationInterface>())
	{
		IG2IActivationInterface::Execute_Deactivate(MovementComponent);
	}
}

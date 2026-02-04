#include "G2IAIControllerDaughter.h"
#include "G2IAIMovementComponent.h"

AG2IAIControllerDaughter::AG2IAIControllerDaughter()
{
	MovementComponent = CreateDefaultSubobject<UG2IAIMovementComponent>(TEXT("MovementComp"));
}

void AG2IAIControllerDaughter::OnPossess(APawn* InPawn)
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

void AG2IAIControllerDaughter::OnUnPossess()
{
	Super::OnUnPossess();

	if (MovementComponent && MovementComponent->Implements<UG2IActivationInterface>())
	{
		IG2IActivationInterface::Execute_Deactivate(MovementComponent);
	}
}

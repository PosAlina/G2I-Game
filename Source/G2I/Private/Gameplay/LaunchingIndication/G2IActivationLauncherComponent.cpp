#include "LaunchingIndication/G2IActivationLauncherComponent.h"
#include "G2IActivationInterface.h"
#include "G2I.h"

void UG2IActivationLauncherComponent::CallActivation()
{
	bActivated = true;

	for (AActor* Actor : ActorsToActivate)
	{
		if (!ensure(Actor))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: ActorsToActivate array contains null Actor."), *GetName());
			continue;
		}

		if (Actor->Implements<UG2IActivationInterface>())
		{
			IG2IActivationInterface::Execute_Activate(Actor);
		}

		TArray<UActorComponent*> Components;
		Actor->GetComponents(Components);

		for (UActorComponent* Component : Components)
		{
			if (Component && Component->Implements<UG2IActivationInterface>())
			{
				IG2IActivationInterface::Execute_Activate(Component);
			}
		}
	}
}

void UG2IActivationLauncherComponent::SetIsLaunched(const bool bNewIsLaunched)
{
	if ((bNewIsLaunched && bActivated) || !bNewIsLaunched)
	{
		Super::SetIsLaunched(bNewIsLaunched);
	}
}

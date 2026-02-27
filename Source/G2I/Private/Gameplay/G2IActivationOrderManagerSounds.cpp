
#include "Gameplay/G2IActivationOrderManagerSounds.h"
#include "Kismet/GameplayStatics.h"

void AG2IActivationOrderManagerSounds::OrderCompleted()
{
	Super::OrderCompleted();
	if (CorrectOrderSound)
		UGameplayStatics::PlaySound2D(this, CorrectOrderSound);
}

void AG2IActivationOrderManagerSounds::OrderFailed()
{
	Super::OrderFailed();
	if (FailedOrderSound)
		UGameplayStatics::PlaySound2D(this, FailedOrderSound);
}

void AG2IActivationOrderManagerSounds::OnActorActivated(AActor* ActivatedActor, bool bReactivation, UG2IActivationWithOrderComponent* ActivationComponent)
{
	if (bReactivation)
	{
		if (CurrentIndex > 0 && ActivatedActor == ActivatedActorsArray.Last())
		{
			ActivatedActorsArray.Pop();
			CurrentIndex--;
			if (ActivationComponent)
				ActivationComponent->Declined();
		}
	}
	else
	{
		ActivatedActorsArray.Add(ActivatedActor);
		if (ActivationComponent)
			ActivationComponent->Accepted(CurrentIndex);
		CurrentIndex++;
		CheckIfOrderCompleted();
	}
}

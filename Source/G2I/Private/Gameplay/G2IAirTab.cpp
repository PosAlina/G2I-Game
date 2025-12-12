
#include "Gameplay/G2IAirTab.h"
#include "Interfaces/G2IActivationInterface.h"

AG2IAirTab::AG2IAirTab()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	if (StaticMeshComponent)
		SetRootComponent(StaticMeshComponent);

	BoxComponent = CreateDefaultSubobject<UG2IPipesBoxComponent>(TEXT("G2IPipesBoxComponent"));
	BoxComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	BoxComponent->Owner = this;
	BoxComponent->bRecieves = true;
	BoxComponent->SetGenerateOverlapEvents(true);
	BoxComponent->SetCollisionObjectType(ECC_GameTraceChannel3);					 // Pipes Custom Collision
	BoxComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap); // Pipes Custom Collision
}

void AG2IAirTab::OnConstruction(const FTransform& Transform)
{
	BoxComponent->SetBoxExtent(BoxExtents);
}

void AG2IAirTab::RecieveAir_Implementation(AActor* sender, bool bAirPassed)
{
	AirSendersMap.Add(sender, bAirPassed);
	ChangeActivated(CheckIfEnoughAir());
}

bool AG2IAirTab::CheckIfEnoughAir()
{
	int counter = 0;

	for (auto& SenderPair : AirSendersMap)
	{
		if (SenderPair.Value)
		{
			if (++counter >= NumOfPipesNeeded)
				return true;
		}
	}

	return false;
}

bool AG2IAirTab::GetActivated() const
{
	return bActivated;
}

void AG2IAirTab::ChangeActivated(bool newActivated)
{
	if (bActivated != newActivated)
	{
		bActivated = newActivated;
		bActivated ? ActivateActors() : DeactivateActors();
	}
}

void AG2IAirTab::ActivateActors()
{
	for (auto& Actor : ActorsToActivate)
		IG2IActivationInterface::Execute_Activate(Actor);

	UE_LOG(LogG2I, Log, TEXT("Activated actors from %s"), *GetActorNameOrLabel());
}

void AG2IAirTab::DeactivateActors()
{
	for (auto& Actor : ActorsToActivate)
		IG2IActivationInterface::Execute_Deactivate(Actor);

	UE_LOG(LogG2I, Log, TEXT("Deactivated actors from %s"), *GetActorNameOrLabel());
}



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
	BoxComponent->Owner = this;

	BoxComponent->SetBoxExtent(BoxExtents);
	if (StaticMeshComponent->GetStaticMesh())
		BoxComponent->SetRelativeLocation(StaticMeshComponent->GetStaticMesh()->GetBounds().GetBox().GetCenter());
}

void AG2IAirTab::RecieveAir_Implementation(AActor* Sender, bool bAirPassed)
{
	UE_LOG(LogG2I, Log, TEXT("RecieveAir called in %s"), *GetActorNameOrLabel());
	AirSendersMap.Add(Sender, bAirPassed);
	ChangeActivated(CheckIfEnoughAir());
}

bool AG2IAirTab::CheckIfEnoughAir()
{
	int Counter = 0;

	for (auto& SenderPair : AirSendersMap)
	{
		if (SenderPair.Value)
		{
			if (++Counter >= NumOfPipesNeeded)
				return true;
		}
	}

	return false;
}

bool AG2IAirTab::GetActivated() const
{
	return bActivated;
}

void AG2IAirTab::ChangeActivated(bool bNewActivated)
{
	if (bActivated != bNewActivated)
	{
		UE_LOG(LogG2I, Log, TEXT("bActivated changed in %s to %d"), *GetActorNameOrLabel(), bNewActivated);
		bActivated = bNewActivated;
		bActivated ? ActivateActors() : DeactivateActors();
	}
}

void AG2IAirTab::ActivateActors()
{
	for (auto& Actor : ActorsToActivate)
		IG2IActivationInterface::Execute_Activate(Actor);

	UE_LOG(LogG2I, Log, TEXT("Activated %d actors from %s"), ActorsToActivate.Num(), *GetActorNameOrLabel());
}

void AG2IAirTab::DeactivateActors()
{
	for (auto& Actor : ActorsToActivate)
		IG2IActivationInterface::Execute_Deactivate(Actor);

	UE_LOG(LogG2I, Log, TEXT("Deactivated %d actors from %s"), ActorsToActivate.Num(), *GetActorNameOrLabel());
}


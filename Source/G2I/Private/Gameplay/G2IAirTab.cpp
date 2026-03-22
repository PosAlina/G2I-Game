#include "Gameplay/G2IAirTab.h"
#include "Interfaces/G2IActivationInterface.h"
#include "G2I.h"

AG2IAirTab::AG2IAirTab()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	if (StaticMeshComponent)
		SetRootComponent(StaticMeshComponent);

	BoxComponent = CreateDefaultSubobject<UG2IPipesBoxComponent>(TEXT("PipesBoxComponent"));
	BoxComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	BoxComponent->Owner = this;
}

void AG2IAirTab::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	BoxComponent->SetBoxExtent(BoxExtents);
	if (StaticMeshComponent->GetStaticMesh())
		BoxComponent->SetRelativeLocation(StaticMeshComponent->GetStaticMesh()->GetBounds().GetBox().GetCenter());
}

void AG2IAirTab::RecieveAir_Implementation(AActor* Sender, bool bAirPassed)
{
	if (!(Sender) || (Sender == this))
		return;

	UE_LOG(LogG2I, Log, TEXT("RecieveAir called in %s with bAirPassed = %d."), *GetActorNameOrLabel(), bAirPassed);
	AirSendersMap.Add(Sender, bAirPassed);
	ChangeActivated(CheckIfEnoughAir());
}

bool AG2IAirTab::CheckIfEnoughAir() const
{
	int32 Counter = 0;

	for (const auto& [_, bHasAir] : AirSendersMap)
	{
		if (bHasAir)
		{
			++Counter;
			if (Counter == NumOfPipesNeeded)
				return true;
		}
	}

	return false;
}

bool AG2IAirTab::GetActivated() const
{
	return bActivated;
}

void AG2IAirTab::ChangeActivated(const bool bNewActivated)
{
	if (bActivated != bNewActivated)
	{
		bActivated = bNewActivated;
		UE_LOG(LogG2I, Log, TEXT("bActivated changed in %s to %d"), *GetActorNameOrLabel(), bNewActivated);

		if (bActivated)
		{
			ActivateActors();
		}
		else
		{
			DeactivateActors();
		}
	}
}

void AG2IAirTab::ActivateActors()
{
	for (const auto& Actor : ActorsToActivate)
	{
		if (Actor && Actor->Implements<UG2IActivationInterface>())
		{
			IG2IActivationInterface::Execute_Activate(Actor);
		}
	}

	UE_LOG(LogG2I, Log, TEXT("Activated %d actors from %s"), ActorsToActivate.Num(), *GetActorNameOrLabel());
}

void AG2IAirTab::DeactivateActors()
{
	for (const auto& Actor : ActorsToActivate)
	{
		if (Actor && Actor->Implements<UG2IActivationInterface>())
		{
			IG2IActivationInterface::Execute_Deactivate(Actor);
		}
	}

	UE_LOG(LogG2I, Log, TEXT("Deactivated %d actors from %s"), ActorsToActivate.Num(), *GetActorNameOrLabel());
}

void AG2IAirTab::BeginPlay()
{
	Super::BeginPlay();

	if (bActivated)
	{
		ActivateActors();
	}
	else 
	{
		DeactivateActors();
	}
}

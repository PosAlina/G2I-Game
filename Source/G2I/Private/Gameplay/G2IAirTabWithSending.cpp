#include "Gameplay/G2IAirTabWithSending.h"
#include "G2I.h"

AG2IAirTabWithSending::AG2IAirTabWithSending()
{
	SendingToPipesComponent = CreateDefaultSubobject<UG2ISendToPipesComponent>(TEXT("G2ISendToPipesComponent"));
	SendingToPipesComponent->SetupAttachment(RootComponent);
	SendingToPipesComponent->bHasAir = bActivated;
}

void AG2IAirTabWithSending::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (SendingToPipesComponent)
	{
		SendingToPipesComponent->PipesBoxComponent->SetBoxExtent(BoxExtents);
	}
}

void AG2IAirTabWithSending::ChangeActivated(const bool bNewActivated)
{
	Super::ChangeActivated(bNewActivated);

	if (SendingToPipesComponent)
	{
		SendingToPipesComponent->SetHasAir(bNewActivated);
	}
}

void AG2IAirTabWithSending::BeginPlay()
{
	Super::BeginPlay();

	if (SendingToPipesComponent)
	{
		SendingToPipesComponent->SendAir();
	}
	else
	{
		UE_LOG(LogG2I, Warning, TEXT("SendingToPipesComponent in null in %s"), *GetActorNameOrLabel());
	}
}

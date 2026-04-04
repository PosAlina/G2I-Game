#include "Components/G2ISendToPipesComponent.h"
#include "G2I.h"
#include "G2IAirReceiverInterface.h"

UG2ISendToPipesComponent::UG2ISendToPipesComponent()
{
	PipesBoxComponent = CreateDefaultSubobject<UG2IPipesBoxComponent>(TEXT("G2IPipesBoxComponent"));
	if (PipesBoxComponent)
	{
		PipesBoxComponent->SetupAttachment(this);
		PipesBoxComponent->bReceives = false;
	}
}

void UG2ISendToPipesComponent::BeginPlay()
{
	Super::BeginPlay();

	if (PipesBoxComponent)
	{
		PipesBoxComponent->Owner = GetOwner();
		PipesBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnPipesBoxBeginOverlap);
		PipesBoxComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnPipesBoxEndOverlap);
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogG2I, Error, TEXT("World is null in %s."), *GetName());
		return;
	}

	// Forcing Overlap Events for pipes to connect to each other
	if (World->GetBegunPlay())
	{
		UE_LOG(LogG2I, Verbose, TEXT("World's BeginPlay already ended during %s's BeginPlay, starting ForceOverlaps"), *GetName());
		ForceOverlaps();
	}
	else
	{
		UE_LOG(LogG2I, Verbose, TEXT("World exists but still hasn't played BeginPlay during %s's BeginPlay, subscribing to OnWorldBeginPlay"), *GetName());
		GetWorld()->OnWorldBeginPlay.AddUObject(this, &ThisClass::ForceOverlaps);
	}
}

void UG2ISendToPipesComponent::ForceOverlaps()
{
	UE_LOG(LogG2I, Log, TEXT("ForceOverlaps called in %s"), *GetName());

	// Force call overlap events
	if (PipesBoxComponent)
	{
		TArray<UPrimitiveComponent*> OverlappingComponents;
		PipesBoxComponent->GetOverlappingComponents(OverlappingComponents);

		for (UPrimitiveComponent* OtherComp : OverlappingComponents)
			OnPipesBoxBeginOverlap(PipesBoxComponent, nullptr, OtherComp, 0, false, FHitResult());
	}

	SendAir();
}

void UG2ISendToPipesComponent::OnPipesBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UG2IPipesBoxComponent* OtherBox = Cast<UG2IPipesBoxComponent>(OtherComp);

	if (ensure(OtherBox))
	{
		if (!OtherBox->Owner)
		{
			UE_LOG(LogG2I, Warning, TEXT("Owner in Box Component %s is NULL."), *OtherBox->GetName());
			return;
		}

		if (OtherBox->Owner == GetOwner())
		{
			UE_LOG(LogG2I, Verbose, TEXT("Attempted to send air to itself in %s."), *GetName());
			return;
		}

		if (OtherBox->bReceives)
			ActorsToSendAirTo.AddUnique(OtherBox->Owner);
	}
}

void UG2ISendToPipesComponent::OnPipesBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherComp)
	{
		return;
	}

	UG2IPipesBoxComponent* OtherBox = Cast<UG2IPipesBoxComponent>(OtherComp);
	if (OtherBox)
	{
		if (OtherBox->bReceives)
			ActorsToSendAirTo.Remove(OtherBox->Owner);
	}
}

void UG2ISendToPipesComponent::SendAir() const
{
	for (int32 i = 0; i < ActorsToSendAirTo.Num(); i++)
	{
		if (!ActorsToSendAirTo[i])
		{
			UE_LOG(LogG2I, Warning, TEXT("Attempted to send air to null actor at index %i in %s."), i, *GetName());
			continue;
		}

		if (ActorsToSendAirTo[i]->Implements<UG2IAirReceiverInterface>())
		{
			IG2IAirReceiverInterface::Execute_ReceiveAir(ActorsToSendAirTo[i], GetOwner(), bHasAir);
		}
		else
		{
			UE_LOG(LogG2I, Warning, TEXT("Actor %s doesn't implement interface G2IAirReceiverInterface."), *ActorsToSendAirTo[i]->GetActorNameOrLabel());
		}
	}
}

void UG2ISendToPipesComponent::SetHasAir(const bool bNewHasAir)
{
	if (bHasAir != bNewHasAir)
	{
		bHasAir = bNewHasAir;
		SendAir();
	}
}


#include "Gameplay/G2ITechnicalHole.h"
#include "Characters/G2ICharacterEngineer.h"
#include "Components/G2IHoleInteractionComponent.h"
#include "Gameplay/G2IPipe.h"

// Sets default values
AG2ITechnicalHole::AG2ITechnicalHole()
{
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	if (RootSceneComponent)
		SetRootComponent(RootSceneComponent);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->AttachToComponent(RootSceneComponent, FAttachmentTransformRules::KeepRelativeTransform);
	BoxComponent->SetGenerateOverlapEvents(true);
	BoxComponent->SetBoxExtent(FVector(15.f));

	Tags.Add(TEXT("Interactive1"));
}

bool AG2ITechnicalHole::CanInteract_Implementation(const ACharacter* Interactor)
{
	if (Interactor->FindComponentByClass<UG2IHoleInteractionComponent>())
		return true;

	return false;
}

void AG2ITechnicalHole::Interact_Implementation(const ACharacter* Interactor)
{
	StartSendingAir();
}

void AG2ITechnicalHole::StartSendingAir()
{
	if (AG2IPipe* Pipe = Cast<AG2IPipe>(Owner))
	{
		Pipe->SetHasTechnicalHoleAtSplinePoint(PointIndex, false);
		Pipe->CheckIfAirCanPass();

		GetWorldTimerManager().SetTimer(TimerHandle, this, &AG2ITechnicalHole::StopSendingAir, TimeToSendAir, false);
		UE_LOG(LogG2I, Log, TEXT("%s: Started timer (%f seconds)."), *GetActorNameOrLabel(), TimeToSendAir);
	}
	else
		UE_LOG(LogG2I, Error, TEXT("Couldn't cast Owner to AG2IPipe in %s"), *GetActorNameOrLabel());
}

void AG2ITechnicalHole::StopSendingAir()
{
	if (AG2IPipe* Pipe = Cast<AG2IPipe>(Owner))
	{
		Pipe->SetHasTechnicalHoleAtSplinePoint(PointIndex, true);
		Pipe->CheckIfAirCanPass();

		UE_LOG(LogG2I, Log, TEXT("%s: Timer stopped."), *GetActorNameOrLabel());
	}
	else
		UE_LOG(LogG2I, Error, TEXT("Couldn't cast Owner to AG2IPipe in %s"), *GetActorNameOrLabel());
}

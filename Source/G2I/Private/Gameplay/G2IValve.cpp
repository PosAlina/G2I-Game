
#include "Gameplay/G2IValve.h"
#include "G2I.h"
#include "Characters/G2ICharacterEngineer.h"
#include "Components/G2IValveInteractionComponent.h"
#include "Gameplay/G2IPipe.h"

AG2IValve::AG2IValve()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh Component"));
	StaticMeshComponent->SetGenerateOverlapEvents(true);

	if (StaticMeshComponent)
		SetRootComponent(StaticMeshComponent);
}

void AG2IValve::BeginPlay()
{
	Super::BeginPlay();
	PassActivationToPipe();
}

bool AG2IValve::CanInteract_Implementation(const ACharacter* Interactor)
{
	if (Interactor->FindComponentByClass<UG2IValveInteractionComponent>())
		return true;

	return false;
}

void AG2IValve::Interact_Implementation(const ACharacter* Interactor)
{
	bActivated = !bActivated;
	UE_LOG(LogG2I, Log, TEXT("%s Activation: %d"), *GetActorNameOrLabel(), bActivated);

	// TODO play animation
	// TODO play sound

	PassActivationToPipe();
}

void AG2IValve::PassActivationToPipe()
{
	if (AG2IPipe* Pipe = Cast<AG2IPipe>(OwnerActor))
	{
		Pipe->OnValveActivationChanged(this, bActivated);
	}
}

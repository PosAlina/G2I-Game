
#include "Gameplay/G2IValve.h"
#include "G2I.h"
#include "Characters/G2ICharacterEngineer.h"
#include "Components/G2IValveInteractionComponent.h"
#include "Gameplay/G2IPipe.h"

AG2IValve::AG2IValve()
{
	PrimaryActorTick.TickInterval = 0.05f;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh Component"));
	StaticMeshComponent->SetGenerateOverlapEvents(true);
	StaticMeshComponent->SetMobility(EComponentMobility::Movable);

	if (SceneRootComponent)
	{
		SetRootComponent(SceneRootComponent);
		if (StaticMeshComponent)
			StaticMeshComponent->AttachToComponent(SceneRootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}
}

void AG2IValve::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ApplyLocalRotation();
}

void AG2IValve::BeginPlay()
{
	Super::BeginPlay();
	DeltaRotation *= -1.;
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
	ChangeActivation();
	PassActivationToPipe();
}

void AG2IValve::PassActivationToPipe()
{
	UE_LOG(LogG2I, Verbose, TEXT("%s PassActivationToPipe called"), *GetActorNameOrLabel());

	if (AG2IPipe* Pipe = Cast<AG2IPipe>(OwnerActor))
	{
		Pipe->OnValveActivationChanged(this, bActivated);
	}
}

void AG2IValve::ApplyLocalRotation()
{
	StaticMeshComponent->AddLocalRotation(DeltaRotation);
	CurrentRotation += DeltaRotation;

	if (CurrentRotation.Pitch > MaxRotation.Pitch ||
		CurrentRotation.Roll > MaxRotation.Roll ||
		CurrentRotation.Yaw > MaxRotation.Yaw)
	{
		CurrentRotation = MaxRotation;
		SetActorTickEnabled(false);
	}

	if (CurrentRotation.Pitch < MinRotation.Pitch ||
		CurrentRotation.Roll < MinRotation.Roll ||
		CurrentRotation.Yaw < MinRotation.Yaw)
	{
		CurrentRotation = MinRotation;
		SetActorTickEnabled(false);
	}
}

void AG2IValve::ChangeActivation()
{
	bActivated = !bActivated;
	DeltaRotation *= -1.;
	UE_LOG(LogG2I, Log, TEXT("%s Activation: %d"), *GetActorNameOrLabel(), bActivated);

	if (StaticMeshComponent)
		SetActorTickEnabled(true);

	// TODO play sound
}

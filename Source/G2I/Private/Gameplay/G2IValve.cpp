#include "Gameplay/G2IValve.h"
#include "G2I.h"
#include "G2IWorldHintKeyWidgetComponent.h"
#include "Characters/G2ICharacterEngineer.h"
#include "Components/G2IValveInteractionComponent.h"
#include "Gameplay/G2IPipe.h"

AG2IValve::AG2IValve()
{
	PrimaryActorTick.TickInterval = 0.01f;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	Tags.Add(TEXT("Interactive1"));

	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	if (SceneRootComponent)
	{
		SetRootComponent(SceneRootComponent);
	}

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh Component"));
	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetupAttachment(RootComponent);
		StaticMeshComponent->SetGenerateOverlapEvents(true);
		StaticMeshComponent->SetMobility(EComponentMobility::Movable);
	}

	HintKeyWidgetComp = CreateDefaultSubobject<UG2IWorldHintKeyWidgetComponent>(TEXT("HintKeyWidget"));
	if (!ensure(HintKeyWidgetComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create %s"), *GetActorNameOrLabel(),
			*UG2IWorldHintKeyWidgetComponent::StaticClass()->GetName());
	}
	else
	{
		HintKeyWidgetComp->SetupAttachment(RootComponent);
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
	if (!ensure(HintKeyWidgetComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
			*UG2IWorldHintKeyWidgetComponent::StaticClass()->GetName());
		return;
	}
	HintKeyWidgetComp->SetIsLocked_Implementation(bIsLocked);
}

bool AG2IValve::CanInteract_Implementation(const ACharacter* Interactor)
{
	if (bIsLocked)
	{
		return false;
	}
	if (Interactor->FindComponentByClass<UG2IValveInteractionComponent>())
		return true;

	return false;
}

void AG2IValve::Interact_Implementation(const ACharacter* Interactor)
{
	ChangeActivation();
	PassActivationToPipe();
}

UG2IWorldHintKeyWidgetComponent* AG2IValve::GetInteractionKeyHintComponent_Implementation()
{
	return HintKeyWidgetComp;
}

void AG2IValve::SetIsLocked_Implementation(const bool bIsNewLocked)
{
	bIsLocked = bIsNewLocked;

	if (!ensure(HintKeyWidgetComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
			*UG2IWorldHintKeyWidgetComponent::StaticClass()->GetName());
		return;
	}
	HintKeyWidgetComp->SetIsLocked_Implementation(bIsLocked);
}

bool AG2IValve::IsLocked_Implementation()
{
	return bIsLocked;
}

void AG2IValve::PassActivationToPipe()
{
	UE_LOG(LogG2I, Verbose, TEXT("%s PassActivationToPipe called"), *GetActorNameOrLabel());

	if (AG2IPipe* Pipe = Cast<AG2IPipe>(OwnerActor))
	{
		Pipe->OnValveActivationChanged(this, bActivated);
	}
}

void AG2IValve::AddRotationToStaticMesh(const FRotator& Rotation) const
{
	if (!ensure(StaticMeshComponent))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Couldn't rotate StaticMeshComponent."), *GetActorNameOrLabel());
	}
	else
	{
		StaticMeshComponent->AddRelativeRotation(Rotation);
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

	// TODO: play sound
}

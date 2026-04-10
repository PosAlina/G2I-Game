#include "Gameplay/G2IButtonActivator.h"
#include "Interfaces/G2IActivationInterface.h"
#include "G2IWorldHintKeyWidgetComponent.h"
#include "LaunchingIndication/G2ILauncherComponent.h"
#include "G2I.h"
#include "G2IButtonActivator.h"


void AG2IButtonActivator::BeginPlay()
{
	Super::BeginPlay();

	if (!ensure(HintKeyWidgetComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create %s"), *GetActorNameOrLabel(),
			*UG2IWorldHintKeyWidgetComponent::StaticClass()->GetName());
		return;
	}
	if (!ensure(StaticMeshComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create %s"), *GetActorNameOrLabel(),
			*UStaticMeshComponent::StaticClass()->GetName());
	}
	StaticMeshComponent->SetGenerateOverlapEvents(true);
	StaticMeshComponent->SetMobility(EComponentMobility::Movable);
	if (!ensure(LauncherComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
			*UG2ILauncherComponent::StaticClass()->GetName());
		return;
	}
	LauncherComp->SetHintKeyWidget(HintKeyWidgetComp);
}

AG2IButtonActivator::AG2IButtonActivator()
{
	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	if (!ensure(SceneRootComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create Root Component"), *GetActorNameOrLabel());
	}
	else
	{
		SetRootComponent(SceneRootComponent);
	}
	
	LauncherComp = CreateDefaultSubobject<UG2ILauncherComponent>(TEXT("LauncherComp"));
	if (!ensure(LauncherComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create %s"), *GetActorNameOrLabel(),
			*UG2ILauncherComponent::StaticClass()->GetName());
	}

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	if (!ensure(StaticMeshComponent))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create %s"), *GetActorNameOrLabel(),
			*UStaticMeshComponent::StaticClass()->GetName());
	}
	StaticMeshComponent->SetupAttachment(RootComponent);

	HintKeyWidgetComp = CreateDefaultSubobject<UG2IWorldHintKeyWidgetComponent>(TEXT("HintKeyWidget"));
	if (!ensure(HintKeyWidgetComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create %s"), *GetActorNameOrLabel(),
			*UG2IWorldHintKeyWidgetComponent::StaticClass()->GetName());
		return;
	}
	HintKeyWidgetComp->SetupAttachment(StaticMeshComponent);
}

bool AG2IButtonActivator::CanInteract_Implementation(const ACharacter* Interactor)
{
	if (!ensure(LauncherComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
			*UG2ILauncherComponent::StaticClass()->GetName());
		return false;
	}
	if (LauncherComp->IsLocked_Implementation())
	{
		return false;
	}
	return true;
}

void AG2IButtonActivator::Interact_Implementation(const ACharacter* Interactor)
{
	for (AActor* Actor : ActorsToActivate)
	{
		if (!ensure(Actor))
		{
			UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Actor in ActorsToActivate"), *GetActorNameOrLabel());
			continue;
		}

		if (Actor->Implements<UG2IActivationInterface>())
		{
			IG2IActivationInterface::Execute_Activate(Actor);
		}

		TArray<UActorComponent*> Components;
		Actor->GetComponents(Components);

		for (UActorComponent* Component : Components)
		{
			if (Component && Component->Implements<UG2IActivationInterface>())
			{
				IG2IActivationInterface::Execute_Activate(Component);
			}
		}
	}
}

UG2IWorldHintKeyWidgetComponent* AG2IButtonActivator::GetInteractionKeyHintComponent_Implementation()
{
	return HintKeyWidgetComp;
}
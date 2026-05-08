#include "TransitBetweenLevels/G2ITransitBetweenLevelThroughInteraction.h"
#include "G2I.h"
#include "G2IWorldHintKeyWidgetComponent.h"
#include "LaunchingIndication/G2ILauncherComponent.h"
#include "TransitBetweenLevels/G2ITransitBetweenLevelsComponent.h"

AG2ITransitBetweenLevelThroughInteraction::AG2ITransitBetweenLevelThroughInteraction()
{
	TransitComp = CreateDefaultSubobject<UG2ITransitBetweenLevelsComponent>("TransitComp");
	if (!ensure(TransitComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create %s"), *GetActorNameOrLabel(),
			*UG2ITransitBetweenLevelsComponent::StaticClass()->GetName());
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

void AG2ITransitBetweenLevelThroughInteraction::BeginPlay()
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

bool AG2ITransitBetweenLevelThroughInteraction::CanInteract_Implementation(const ACharacter* Interactor)
{
	if (!ensure(LauncherComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
			*UG2ILauncherComponent::StaticClass()->GetName());
		return false;
	}
	if (!ensure(TransitComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create %s"), *GetActorNameOrLabel(),
			*UG2ITransitBetweenLevelsComponent::StaticClass()->GetName());
		return false;
	}
	if (LauncherComp->IsLocked_Implementation())
	{
		return false;
	}
	if (TransitComp->BanCharacters.Contains(Interactor))
	{
		return false;
	}
	return true;
}

void AG2ITransitBetweenLevelThroughInteraction::Interact_Implementation(const ACharacter* Interactor)
{
	if (!ensure(TransitComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Transit Component can't create"), *GetActorNameOrLabel());
		return;
	}
	if (!ensure(TransitComp->Transit()))
	{
		G2I::DebugWarningMessage(GetActorNameOrLabel() + " can't transit to level");
	}
}

UG2IWorldHintKeyWidgetComponent *
	AG2ITransitBetweenLevelThroughInteraction::GetInteractionKeyHintComponent_Implementation()
{
	return HintKeyWidgetComp;
}
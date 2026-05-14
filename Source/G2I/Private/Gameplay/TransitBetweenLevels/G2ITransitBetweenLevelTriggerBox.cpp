#include "TransitBetweenLevels/G2ITransitBetweenLevelTriggerBox.h"
#include "G2I.h"
#include "G2IPlayerController.h"
#include "LaunchingIndication/G2ILauncherComponent.h"
#include "TransitBetweenLevels/G2ITransitBetweenLevelsComponent.h"

AG2ITransitBetweenLevelTriggerBox::AG2ITransitBetweenLevelTriggerBox()
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
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
			*UG2ILauncherComponent::StaticClass()->GetName());
	}
}

void AG2ITransitBetweenLevelTriggerBox::BeginPlay()
{
	Super::BeginPlay();

#if WITH_EDITOR
	SetHidden(false);
#endif
	if (!ensure(LauncherComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
			*UG2ILauncherComponent::StaticClass()->GetName());
		return;
	}
	LauncherComp->GetOnLockedDelegate().AddDynamic(this, &ThisClass::Locked);
}

void AG2ITransitBetweenLevelTriggerBox::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (bIsLocked)
	{
		return;
	}
	Super::NotifyActorBeginOverlap(OtherActor);

	const UWorld *World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World is null in %s"), *GetActorNameOrLabel());
		return;
	}
	const AG2IPlayerController *PlayerController = Cast<AG2IPlayerController>(World->GetFirstPlayerController());
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Player Controller isn't %s in %s"),
			*AG2IPlayerController::StaticClass()->GetName(), *GetActorNameOrLabel());
		return;
	}
	if (!ensure(TransitComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Transit Component can't create"), *GetActorNameOrLabel());
		return;
	}
	APawn *PlayerPawn = PlayerController->GetPawn();
	if (PlayerPawn == OtherActor)
	{
		if (TransitComp->BanCharacters.Contains(PlayerPawn))
		{
			return;
		}
		if (!ensure(TransitComp->Transit()))
		{
			G2I::DebugWarningMessage(GetActorNameOrLabel() + " can't transit to level");
		}
	}
}

void AG2ITransitBetweenLevelTriggerBox::SetIsLocked_Implementation(const bool bIsNewLocked)
{
	bIsLocked = bIsNewLocked;
}

bool AG2ITransitBetweenLevelTriggerBox::IsLocked_Implementation()
{
	return bIsLocked;
}

void AG2ITransitBetweenLevelTriggerBox::Locked(UG2ILauncherComponent* LauncherComponent, AActor* ComponentOwner,
	const bool bIsNewLocked)
{
	SetIsLocked_Implementation(bIsLocked);
}
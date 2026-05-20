#include "G2IOverlapActorsActivator.h"
#include "G2I.h"
#include "G2IActivationInterface.h"
#include "LaunchingIndication/G2IActivationLauncherComponent.h"
#include "Kismet/GameplayStatics.h"

void AG2IOverlapActorsActivator::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!ensure(OtherActor))
	{
		UE_LOG(LogG2I, Warning, TEXT("Began overlap with nullptr actor in %s"), *GetActorNameOrLabel());
		return;
	}
	
	if (bEnableActivateBeginOverlappingActors)
	{
		if (OtherActor->Implements<UG2IActivationInterface>() && OtherActor->ActorHasTag(CheckerTag))
		{
			IG2IActivationInterface::Execute_Activate(OtherActor);

			if (!ensure(LauncherComp))
			{
				UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
					*UG2ILauncherComponent::StaticClass()->GetName());
			}
			else
			{
				if (LauncherComp->bSkipActivation)
				{
					LauncherComp->LaunchWithoutActivation(true);
				}
				else
				{
					LauncherComp->CallActivation();
				}
			}

			const FString DebugMessage = GetActorNameOrLabel() + " in " + OtherActor->GetActorNameOrLabel() + " activated";
			G2I::DebugLogMessage(DebugMessage);

			if (!ensure(World))
			{
				UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetActorNameOrLabel());
				return;
			}
			if (ActivateSound)
			{
				UGameplayStatics::PlaySound2D(World, ActivateSound);
			}
		}
	}
}

void AG2IOverlapActorsActivator::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (!ensure(OtherActor))
	{
		UE_LOG(LogG2I, Warning, TEXT("Ended overlap with nullptr actor in %s"), *GetActorNameOrLabel());
		return;
	}

	if (bEnableDeactivateEndOverlappingActors)
	{
		if (OtherActor->Implements<UG2IActivationInterface>() && OtherActor->ActorHasTag(CheckerTag))
		{
			IG2IActivationInterface::Execute_Deactivate(OtherActor);

			const FString DebugMessage = GetActorNameOrLabel() + " in " + OtherActor->GetActorNameOrLabel() + " deactivated";
			G2I::DebugLogMessage(DebugMessage);
			
			if (!ensure(World))
			{
				UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetActorNameOrLabel());
				return;
			}
			if (DeactivateSound)
			{
				UGameplayStatics::PlaySound2D(World, DeactivateSound);
			}
		}
	}
}

void AG2IOverlapActorsActivator::BeginPlay()
{
	Super::BeginPlay();

	World =  GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetActorNameOrLabel());
		return;
	}

	if (bEnableActivateBeginOverlappingActors)
	{
		TArray<AActor *> OverlappingActors;
		GetOverlappingActors(OverlappingActors);
		for (AActor *OtherActor : OverlappingActors)
		{
			if (!ensure(OtherActor))
			{
				UE_LOG(LogG2I, Warning, TEXT("Nullptr overlapping actor in %s's BeginPlay"), *GetActorNameOrLabel());
				return;
			}
			
			if (OtherActor->Implements<UG2IActivationInterface>() && OtherActor->ActorHasTag(CheckerTag))
			{
				IG2IActivationInterface::Execute_Activate(OtherActor);
				if (!ensure(LauncherComp))
				{
					UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetActorNameOrLabel(),
						*UG2ILauncherComponent::StaticClass()->GetName());
				}
				else
				{
					if (LauncherComp->bSkipActivation)
					{
						LauncherComp->LaunchWithoutActivation(true);
					}
					else
					{
						LauncherComp->CallActivation();
					}
				}

				const FString DebugMessage = GetActorNameOrLabel() + " in " + OtherActor->GetActorNameOrLabel() + " activated";
				G2I::DebugLogMessage(DebugMessage);
			}
		}
	}
}

AG2IOverlapActorsActivator::AG2IOverlapActorsActivator() {
	LauncherComp = CreateDefaultSubobject<UG2IActivationLauncherComponent>(TEXT("LauncherComp"));
	if (!ensure(LauncherComp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create %s"), *GetActorNameOrLabel(),
			*UG2ILauncherComponent::StaticClass()->GetName());
	}
}
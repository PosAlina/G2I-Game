#include "G2IOverlapActorChecker.h"
#include "G2I.h"
#include "G2IActivationInterface.h"
#include "Kismet/GameplayStatics.h"

void AG2IOverlapActorChecker::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (bEnableActivateBeginOverlappingActors)
	{
		if (OtherActor->Implements<UG2IActivationInterface>() && OtherActor->ActorHasTag(CheckerTag))
		{
			IG2IActivationInterface::Execute_Activate(OtherActor);

			if (GEngine)
			{
				const FString DebugMessage = OtherActor->GetActorNameOrLabel() + " activated";
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, DebugMessage);
			}
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

void AG2IOverlapActorChecker::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (bEnableDeactivateEndOverlappingActors)
	{
		if (OtherActor->Implements<UG2IActivationInterface>() && OtherActor->ActorHasTag(CheckerTag))
		{
			IG2IActivationInterface::Execute_Deactivate(OtherActor);

			if (GEngine)
			{
				const FString DebugMessage = OtherActor->GetActorNameOrLabel() + " deactivated";
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, DebugMessage);
			}
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

void AG2IOverlapActorChecker::BeginPlay()
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
			if (OtherActor->Implements<UG2IActivationInterface>() && OtherActor->ActorHasTag(CheckerTag))
			{
				IG2IActivationInterface::Execute_Activate(OtherActor);
				if (GEngine)
				{
					FString DebugMessage = OtherActor->GetActorNameOrLabel() + " activated";
					GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, DebugMessage);
				}
			}
		}
	}
}
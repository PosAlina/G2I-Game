#include "G2IActorsActivatorByOverlappingActors.h"

#include "G2I.h"
#include "G2IActivationInterface.h"
#include "Kismet/GameplayStatics.h"

void AG2IActorsActivatorByOverlappingActors::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (bEnableActivateBeginOverlappingActors)
	{
		if (ActivateActorsByActor(OtherActor) && ActivateSound)
		{
			UGameplayStatics::PlaySound2D(World, ActivateSound);
		}
	}
}

void AG2IActorsActivatorByOverlappingActors::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (bEnableDeactivateEndOverlappingActors)
	{
		if (DeactivateActorsByActor(OtherActor) && DeactivateSound)
		{
			UGameplayStatics::PlaySound2D(World, DeactivateSound);
		}
	}
}

bool AG2IActorsActivatorByOverlappingActors::ActivateActorsByActor(const AActor *Activator)
{
	bool bActivationOccured = false;
	if (!ensure(Activator))
	{
		UE_LOG(LogG2I, Warning, TEXT("In %s begin overlap nullptr actor"), *GetActorNameOrLabel());
		return bActivationOccured;
	}

	for (const auto& [ActivatedTag, ActivatedActors] : ActivatedActorsByCheckerTag)
	{
		if (Activator->ActorHasTag(ActivatedTag))
		{
			if (GEngine)
			{
				const FString DebugMessage = "Activator " + Activator->GetActorNameOrLabel() +
					" with tag " + ActivatedTag.ToString() + " came";
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, DebugMessage);
			}

			for (auto ActivatedActorIt =
				ActivatedActorsByCheckerTag[ActivatedTag].SetOfActivatedActors.CreateIterator();
				ActivatedActorIt; ++ActivatedActorIt)
			{
				if (!(*ActivatedActorIt))
				{
					UE_LOG(LogG2I, Warning, TEXT("In %s set of activated by tag %s actors consisted"
								  " nullptr actor"), *GetActorNameOrLabel(), *ActivatedTag.ToString());
					ActivatedActorIt.RemoveCurrent();
					continue;
				}
				
				if ((*ActivatedActorIt)->Implements<UG2IActivationInterface>())
				{
					IG2IActivationInterface::Execute_Activate(*ActivatedActorIt);
					if (GEngine)
					{
						FString DebugMessage = (*ActivatedActorIt)->GetActorNameOrLabel() + " activated";
						GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, DebugMessage);
					}
					bActivationOccured = true;
				}
				else
				{
					UE_LOG(LogG2I, Warning, TEXT("In %s set of activated by tag %s actors consisted "
								  "unactivated actor %s"), *GetActorNameOrLabel(), *ActivatedTag.ToString(),
								  *(*ActivatedActorIt)->GetActorNameOrLabel());
					ActivatedActorIt.RemoveCurrent();
					continue;
				}
			}
		}
	}

	return bActivationOccured;
}

bool AG2IActorsActivatorByOverlappingActors::DeactivateActorsByActor(const AActor *Activator)
{
	bool bActivationOccured = false;
	if (!ensure(Activator))
	{
		UE_LOG(LogG2I, Warning, TEXT("In %s end overlap nullptr actor"), *GetActorNameOrLabel());
		return bActivationOccured;
	}
	
	for (const auto& [ActivatedTag, ActivatedActors] : ActivatedActorsByCheckerTag)
	{
		if (Activator->ActorHasTag(ActivatedTag))
		{
			if (GEngine)
			{
				const FString DebugMessage = "Activator " + Activator->GetActorNameOrLabel() +
					" with tag " + ActivatedTag.ToString() + " left";
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, DebugMessage);
			}
				
			for (auto ActivatedActorIt =
				ActivatedActorsByCheckerTag[ActivatedTag].SetOfActivatedActors.CreateIterator();
				ActivatedActorIt; ++ActivatedActorIt)
			{
				if (!(*ActivatedActorIt))
				{
					UE_LOG(LogG2I, Warning, TEXT("In %s set of activated by tag %s actors consisted"
								  " nullptr actor"), *GetActorNameOrLabel(), *ActivatedTag.ToString());
					ActivatedActorIt.RemoveCurrent();
					continue;
				}
				
				if ((*ActivatedActorIt)->Implements<UG2IActivationInterface>())
				{
					IG2IActivationInterface::Execute_Deactivate(*ActivatedActorIt);
					if (GEngine)
					{
						FString DebugMessage = (*ActivatedActorIt)->GetActorNameOrLabel() + " deactivated";
						GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, DebugMessage);
					}
					bActivationOccured = true;
				}
				else
				{
					UE_LOG(LogG2I, Warning, TEXT("In %s set of activated by tag %s actors consisted "
								  "unactivated actor %s"), *GetActorNameOrLabel(), *ActivatedTag.ToString(),
								  *(*ActivatedActorIt)->GetActorNameOrLabel());
					ActivatedActorIt.RemoveCurrent();
					continue;
				}
			}
		}
	}

	return bActivationOccured;
}

void AG2IActorsActivatorByOverlappingActors::BeginPlay()
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
		for (const AActor *OtherActor : OverlappingActors)
		{
			if (!ensure(OtherActor))
			{
				UE_LOG(LogG2I, Warning, TEXT("In %s in begin play overlap nullptr actor"), *GetActorNameOrLabel());
				return;
			}
			
			ActivateActorsByActor(OtherActor);
		}
	}
}

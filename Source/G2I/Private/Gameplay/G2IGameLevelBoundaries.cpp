#include "G2IGameLevelBoundaries.h"
#include "G2I.h"
#include "Components/ArrowComponent.h"
#include "Engine/TargetPoint.h"

void AG2IGameLevelBoundaries::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (!ensure(OtherActor))
	{
		UE_LOG(LogG2I, Warning, TEXT("Impossible for nullptr actor to go beyond boundaries in %s"),
			*GetActorNameOrLabel());
		return;
	}

	if (ExitTriggerRespawnLocations.Contains(OtherActor->GetClass()))
	{
		if (!ensure(ExitTriggerRespawnLocations[OtherActor->GetClass()]))
		{
			UE_LOG(LogG2I, Error,
				TEXT("There is no respawn point for the actor %s within game level boundaries %s"),
				*OtherActor->GetActorNameOrLabel(), *GetActorNameOrLabel());
			return;
		}

		for (const ATriggerBox *Exit : ExitTriggerBoxes)
		{
			if (Exit && Exit->IsOverlappingActor(OtherActor))
			{
				return;
			}
		}

		const FVector RespawnLocation = ExitTriggerRespawnLocations[OtherActor->GetClass()]->GetActorLocation();
		if (bUseTargetPointRotation)
		{
			const FRotator RespawnRotation = ExitTriggerRespawnLocations[OtherActor->GetClass()]->ArrowComponent->GetComponentRotation();
			OtherActor->SetActorLocationAndRotation(RespawnLocation, RespawnRotation);
		}
		else
		{
			OtherActor->SetActorLocation(RespawnLocation);
		}
	}
}

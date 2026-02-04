


#include "AnimNotify/G2IPunchNotify.h"

#include "G2I.h"
#include "G2IDestructibleActorInterface.h"
#include "Components/SteamGlove/G2IGlovePunchComponent.h"

void UG2IPunchNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                             const FAnimNotifyEventReference& EventReference)
{

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		UE_LOG(LogG2I, Warning, TEXT("Owner is null"));
		return;
	}

	UG2IGlovePunchComponent* GlovePunchComp = Owner->FindComponentByClass<UG2IGlovePunchComponent>();
	if (!GlovePunchComp)
	{
		UE_LOG(LogG2I, Warning, TEXT("GlovePunchComp is null"));
		return;
	}
	
	GlovePunchComp->ActivatePunch();
	for (auto ActorToDestroy : GlovePunchComp->GetActorsToDestroy())
	{
		if (ActorToDestroy->Implements<UG2IDestructibleActorInterface>())
		{
			IG2IDestructibleActorInterface::Execute_DestroySelf(ActorToDestroy);
		}
	}
	
	GlovePunchComp->ClearActorsToDestroy();
}

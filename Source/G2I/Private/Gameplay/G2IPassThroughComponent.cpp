#include "G2IPassThroughComponent.h"
#include "G2I.h"
#include "G2ICharacterCollisionComponent.h"
#include "G2INavAreaBlockPassableObjects.h"
#include "NavModifierComponent.h"

UG2IPassThroughComponent::UG2IPassThroughComponent()
{
	SetAreaClass(UG2INavAreaBlockPassableObjects::StaticClass());
}

void UG2IPassThroughComponent::OnRegister()
{
	Super::OnRegister();

	PreInitializeDefaults();
}

void UG2IPassThroughComponent::PreInitializeDefaults()
{
	const AActor *Owner = GetOwner();
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner doesn't exist in %s"), *GetName());
		return;
	}

	for (UActorComponent *ActorComponent : Owner->GetComponents())
	{
		if (ActorComponent->IsA<UNavModifierComponent>())
		{
			continue;
		}
		
		UPrimitiveComponent *Primitive = Cast<UPrimitiveComponent>(ActorComponent);
		if (Primitive && Primitive->IsCollisionEnabled())
		{
			Primitive->SetCollisionResponseToChannel(UG2ICharacterCollisionComponent::GetPassingThroughCollisionChannel(), ECollisionResponse::ECR_Ignore);
		}

		ActorComponent->SetCanEverAffectNavigation(false);
	}

	FVector Origin;
	Owner->GetActorBounds(true, Origin, FailsafeExtent, false);
}


#include "Components/G2IPipesBoxComponent.h"

UG2IPipesBoxComponent::UG2IPipesBoxComponent()
{
	bWantsInitializeComponent = true;
}

void UG2IPipesBoxComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetGenerateOverlapEvents(true);
	SetCollisionObjectType(ECC_GameTraceChannel3);					 // Pipes Custom Collision
	SetCollisionResponseToAllChannels(ECR_Ignore);
	SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap); // Pipes Custom Collision
}

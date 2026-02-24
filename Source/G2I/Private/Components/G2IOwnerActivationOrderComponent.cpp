
#include "Components/G2IOwnerActivationOrderComponent.h"

void UG2IOwnerActivationOrderComponent::BeginPlay()
{
	OwnerActor = GetOwner();
}

void UG2IOwnerActivationOrderComponent::Accepted(int32 AtIndex)
{
	bAccepted = true;
	if (OwnerActor && OwnerActor->Implements<IG2IActivationInterface>())
		IG2IActivationInterface::Execute_Activate(OwnerActor);
}

void UG2IOwnerActivationOrderComponent::Declined()
{
	bAccepted = false;
	if (OwnerActor && OwnerActor->Implements<IG2IActivationInterface>())
		IG2IActivationInterface::Execute_Activate(OwnerActor);
}

void UG2IOwnerActivationOrderComponent::Restored()
{
	bActivated = false;
	if (OwnerActor && OwnerActor->Implements<IG2IActivationInterface>())
		IG2IActivationInterface::Execute_Deactivate(OwnerActor);
}

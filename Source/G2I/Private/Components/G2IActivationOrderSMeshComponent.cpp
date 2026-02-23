
#include "Components/G2IActivationOrderSMeshComponent.h"

void UG2IActivationOrderSMeshComponent::BeginPlay()
{
	OwnerActor = GetOwner();
}

void UG2IActivationOrderSMeshComponent::Accepted(int32 AtIndex)
{
	bAccepted = true;
	if (OwnerActor && OwnerActor->Implements<IG2IActivationInterface>())
		IG2IActivationInterface::Execute_Activate(OwnerActor);
}

void UG2IActivationOrderSMeshComponent::Declined()
{
	bAccepted = false;
	if (OwnerActor && OwnerActor->Implements<IG2IActivationInterface>())
		IG2IActivationInterface::Execute_Activate(OwnerActor);
}

void UG2IActivationOrderSMeshComponent::Restored()
{
	bActivated = false;
	if (OwnerActor && OwnerActor->Implements<IG2IActivationInterface>())
		IG2IActivationInterface::Execute_Deactivate(OwnerActor);
}

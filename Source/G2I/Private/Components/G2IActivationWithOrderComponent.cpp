
#include "Components/G2IActivationWithOrderComponent.h"

void UG2IActivationWithOrderComponent::Activated()
{
	if (bActivated)
	{
		if (bCanBeReactivated)
		{
			bActivated = false;
			OnActivatedDelegate.ExecuteIfBound(GetOwner(), true, this);
		}
	}
	else
	{
		bActivated = true;
		OnActivatedDelegate.ExecuteIfBound(GetOwner(), false, this);
	}
}

void UG2IActivationWithOrderComponent::Accepted(int32 AtIndex)
{
	ActivatedIndexes.Push(AtIndex);
}

void UG2IActivationWithOrderComponent::Declined()
{
	ActivatedIndexes.Pop();
}

void UG2IActivationWithOrderComponent::Restored()
{
	ActivatedIndexes.Empty();
	bActivated = false;
}




#include "Components/G2IDestructibleComponent.h"
#include "GameFramework/Actor.h"
#include "G2I.h"

void UG2IDestructibleComponent::DestroyOwner()
{
	AActor* Owner = GetOwner();
	if (!ensure(Owner)) {
		UE_LOG(LogG2I, Warning, TEXT("%s couldn't get it's owner"), *this->GetName());
		return;
	}
	Owner->Destroy();

	OnDestroy.Broadcast();
}

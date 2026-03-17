#include "Gameplay/G2ICoal.h"
#include "Components/G2IDestructibleComponent.h"
#include "G2I.h"
#include "GameFramework/Actor.h"

AG2ICoal::AG2ICoal() {
	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetGenerateOverlapEvents(true);

	DestructComp = CreateDefaultSubobject<UG2IDestructibleComponent>(TEXT("DestructibleComponent"));

	if (SceneRootComponent)
	{
		SetRootComponent(SceneRootComponent);
		if (StaticMeshComponent) {
			StaticMeshComponent->SetupAttachment(RootComponent);
		}
	}

	Tags.AddUnique(FName("Coal"));
}

void AG2ICoal::Deactivate_Implementation() {
	return;
}

void AG2ICoal::Activate_Implementation() {
	if (!ensure(DestructComp)) {
		UE_LOG(LogG2I, Warning, TEXT("%s couldn't get Destructible Component"), *GetName());
		return;
	}
	DestructComp->DestroyOwner();
}
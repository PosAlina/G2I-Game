#include "BlockWorldWidgetVisibility/G2IBlockWorldWidgetVisibility.h"
#include "G2I.h"
#include "Components/BoxComponent.h"

AG2IBlockWorldWidgetVisibility::AG2IBlockWorldWidgetVisibility()
{
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	RootComponent = CollisionComponent;
}

void AG2IBlockWorldWidgetVisibility::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	if (!ensure(CollisionComponent))
	{
		G2I::DebugWarningMessage(GetName() + "No CollisionComponent selected");
		return;
	}
	CollisionComponent->SetCollisionProfileName("BlockWorldWidgetVisibility");
}

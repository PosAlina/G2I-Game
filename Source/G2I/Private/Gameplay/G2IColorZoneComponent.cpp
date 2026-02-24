


#include "Gameplay/G2IColorZoneComponent.h"
#include "Components/BoxComponent.h"

UG2IColorZoneComponent::UG2IColorZoneComponent()
{
	ZoneCol = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneCol"));
	ZoneCol->SetupAttachment(this);
}

void UG2IColorZoneComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);

	if (ZoneCol)
	{
		ZoneCol->DestroyComponent();
		ZoneCol = nullptr;
	}
}


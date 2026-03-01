#include "Gameplay/G2IColorZoneComponent.h"

#include "G2I.h"
#include "Components/BoxComponent.h"

UG2IColorZoneComponent::UG2IColorZoneComponent()
{
	ZoneCol = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneCol"));
	
	if (!ensure(ZoneCol))
	{
		UE_LOG(LogG2I, Error, TEXT("ZoneColor was not created in %s"), *GetName());
		return;
	}
	
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





#include "Gameplay/G2ISliderLampComponent.h"
#include "Components/PointLightComponent.h"

UG2ISliderLampComponent::UG2ISliderLampComponent()
{
	LampMesh = CreateDefaultSubobject<UStaticMeshComponent>("LampMesh");
	LampLight = CreateDefaultSubobject<UPointLightComponent>("LampLight");
	
	LampMesh->SetupAttachment(this);
	LampLight->SetupAttachment(this);

	LampLight->AttenuationRadius = 100.0f;
	LampLight->SetVisibility(false);
	LampMesh->SetMaterial(0, LightOffMaterial);
}

void UG2ISliderLampComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);

	if (LampMesh)
	{
		LampMesh->DestroyComponent();
		LampMesh = nullptr;
	}

	if (LampLight)
	{
		LampLight->DestroyComponent();
		LampLight = nullptr;
	}

}

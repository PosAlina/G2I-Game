#include "Lights/G2ICheckerPointLight.h"
#include "G2I.h"
#include "Components/PointLightComponent.h"

AG2ICheckerPointLight::AG2ICheckerPointLight()
{
	Lamp = CreateDefaultSubobject<UPointLightComponent>("Lamp");
	if (!ensure(Lamp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create Lamp"), *GetName());
		return;
	}
	Lamp->SetupAttachment(RootComponent);
}

void AG2ICheckerPointLight::SetVisualActivateLight()
{
	Super::SetVisualActivateLight();
	
	SetVisualLamp(ActivateLampVisualInfo);
}

void AG2ICheckerPointLight::SetVisualDeactivateLight()
{
	Super::SetVisualDeactivateLight();
	
	SetVisualLamp(DeactivateLampVisualInfo);
}

void AG2ICheckerPointLight::SetVisualLamp(const FG2ICheckerPointLightInfo& LampInfo) const
{
	if (!ensure(Lamp))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Couldn't find lamp"), *GetName());
		return;
	}
	
	Lamp->SetLightColor(LampInfo.Color);
}
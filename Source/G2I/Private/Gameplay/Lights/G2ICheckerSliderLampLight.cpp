#include "Lights/G2ICheckerSliderLampLight.h"
#include "G2I.h"

AG2ICheckerSliderLampLight::AG2ICheckerSliderLampLight()
{
	Lamp = CreateDefaultSubobject<UG2ISliderLampComponent>("Lamp");
	if (!ensure(Lamp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create Lamp"), *GetName());
		return;
	}
	if (UStaticMeshComponent *LampMesh = Lamp->LampMesh)
	{
		LampMesh->SetupAttachment(RootComponent);
	}
}

void AG2ICheckerSliderLampLight::SetVisualActivateLight()
{
	Super::SetVisualActivateLight();
	
	SetVisualLamp(ActivateLampEmissiveInfo, ActivateLampFlashingInfo);
}

void AG2ICheckerSliderLampLight::SetVisualDeactivateLight()
{
	Super::SetVisualDeactivateLight();
	
	SetVisualLamp(DeactivateLampEmissiveInfo, DeactivateLampFlashingInfo);
}

void AG2ICheckerSliderLampLight::SetVisualLamp(
	const FG2ILampEmissiveInfo& LampEmissiveInfo, const FG2IFlashingInfo& LampFlashingInfo) const
{
	if (!ensure(Lamp))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Couldn't find lamp"), *GetName());
		return;
	}
	
	Lamp->SetLampEmissiveInfo(LampEmissiveInfo);
	if (LampFlashingInfo.bFlashingIsOn)
	{
		Lamp->SetTimerToFlashing(LampFlashingInfo.Rate);
	}
	else
	{
		Lamp->StopTimerToFlashing();
	}
}

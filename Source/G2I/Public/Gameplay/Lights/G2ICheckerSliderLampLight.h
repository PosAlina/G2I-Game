#pragma once

#include "CoreMinimal.h"
#include "G2ICheckerLight.h"
#include "G2ISliderLampComponent.h"
#include "G2ICheckerSliderLampLight.generated.h"

UCLASS()
class G2I_API AG2ICheckerSliderLampLight : public AG2ICheckerLight
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UG2ISliderLampComponent> Lamp;
	
	UPROPERTY(EditAnywhere, Category = "Activation|Activate|Visual")
	FG2ILampEmissiveInfo ActivateLampEmissiveInfo = {FG2ILampEmissiveInfo(FLinearColor::Green)};
	
	UPROPERTY(EditAnywhere, Category = "Activation|Activate|Visual")
	FG2IFlashingInfo ActivateLampFlashingInfo;
	
	UPROPERTY(EditAnywhere, Category = "Activation|Deactivate|Visual")
	FG2ILampEmissiveInfo DeactivateLampEmissiveInfo = {FG2ILampEmissiveInfo(FLinearColor::Red)};
	
	UPROPERTY(EditAnywhere, Category = "Activation|Deactivate|Visual")
	FG2IFlashingInfo DeactivateLampFlashingInfo;

public:
	
	AG2ICheckerSliderLampLight();
	
protected:
	
	virtual void SetVisualActivateLight() override;
	
	virtual void SetVisualDeactivateLight() override;
	
	void SetVisualLamp(const FG2ILampEmissiveInfo& LampEmissiveInfo, const FG2IFlashingInfo& LampFlashingInfo) const;

};

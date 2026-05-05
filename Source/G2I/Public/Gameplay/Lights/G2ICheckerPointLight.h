#pragma once

#include "CoreMinimal.h"
#include "G2ICheckerLight.h"
#include "G2ICheckerPointLight.generated.h"

struct FG2ILampEmissiveInfo;
class UPointLightComponent;

USTRUCT(BlueprintType)
struct FG2ICheckerPointLightInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FLinearColor Color;
};

UCLASS()
class G2I_API AG2ICheckerPointLight : public AG2ICheckerLight
{
	GENERATED_BODY()

	
protected:
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UPointLightComponent> Lamp;
	
	UPROPERTY(EditAnywhere, Category = "Activation|Activate|Visual")
	FG2ICheckerPointLightInfo ActivateLampVisualInfo = {FG2ICheckerPointLightInfo(FLinearColor::Green)};
	
	UPROPERTY(EditAnywhere, Category = "Activation|Deactivate|Visual")
	FG2ICheckerPointLightInfo DeactivateLampVisualInfo = {FG2ICheckerPointLightInfo(FLinearColor::Red)};

public:
	
	AG2ICheckerPointLight();
	
protected:
	
	virtual void SetVisualActivateLight() override;
	
	virtual void SetVisualDeactivateLight() override;
	
	void SetVisualLamp(const FG2ICheckerPointLightInfo& LampInfo) const;

};

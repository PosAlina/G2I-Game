

#pragma once

#include "CoreMinimal.h"
#include "G2IColorZoneComponent.h"
#include "Components/PrimitiveComponent.h"
#include "G2ISliderLampComponent.generated.h"

class UPointLightComponent;
/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class G2I_API UG2ISliderLampComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

public:
	UG2ISliderLampComponent();
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> LampMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPointLightComponent> LampLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EZoneColor Color = EZoneColor::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterial> LightOnMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterial> LightOffMaterial;
	
};

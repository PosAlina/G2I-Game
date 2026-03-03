

#pragma once

#include "CoreMinimal.h"
#include "G2IColorZoneComponent.h"
#include "Components/PrimitiveComponent.h"
#include "G2ISliderLampComponent.generated.h"

class UPointLightComponent;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class G2I_API UG2ISliderLampComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

public:
	UG2ISliderLampComponent();
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	void SetTimerToIntensity(int IntensityChangeDir);
	void SetTimerToFlashing(int FlashCount, float FlashTime);
	
protected:
	virtual void BeginPlay() override;
	
private:
	void ChangeIntensity(int IntensityChangeDir, float TargetLightIntensity);
	void LampFlashing(int FlashCount);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> LampMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UMaterialInstance> LightMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EZoneColor Color = EZoneColor::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor LampColor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxLightIntensityInCommonColorZone = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxLightIntensityInActivationColorZone = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LightIntensityRate = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float IntensityIncreaseFrequency = 0.05f;
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;
	bool bIsLampFlashing = false;
	int LampMode = 0;

private:
	UPROPERTY()
	TObjectPtr<UWorld> World;
	UPROPERTY()
	FTimerHandle IntensityColorTimer;
	UPROPERTY()
	FTimerHandle FlashingTimer;
	int FlashCounter = 0;
	bool bLampFlashState = true;
};

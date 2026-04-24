#pragma once

#include "CoreMinimal.h"
#include "G2IColorZoneComponent.h"
#include "Components/PrimitiveComponent.h"
#include "G2ISliderLampComponent.generated.h"

DECLARE_DELEGATE(FStopFlashingTimer)

USTRUCT(BlueprintType)
struct FG2ILampEmissiveInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp")
	FLinearColor Color = FLinearColor::Yellow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp")
	bool bIsOn = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp")
	float MaxIntensity = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp")
	float IntensityRate = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp")
	float MultiplePointLightIntensity = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp")
	bool bIsPointLightEnabled = true;
};

USTRUCT(BlueprintType)
struct FG2IFlashingInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp")
	bool bFlashingIsOn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp")
	float Rate = 0.5f;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class G2I_API UG2ISliderLampComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

public:
	UG2ISliderLampComponent();
	//virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

	void SetTimerToIntensity(int32 IntensityChangeDir);
	void StopTimerToIntensity();

	void SetTimerToFlashing(float FlashTime, uint32 FlashCount);
	void SetTimerToFlashing(float FlashTime);
	void StopTimerToFlashing();

	void SetDefaultValues();
	void SetBaseColor(const FLinearColor& NewBaseColor);
	void SetEmissiveColor(const FLinearColor& NewEmissiveColor);
	void SetCurrentEmissiveIntensity(float NewEmissiveIntensity);
	void SetMaxEmissiveIntensity(float NewEmissiveIntensity);
	
	void OnLamp();
	void OffLamp();

	FLinearColor GetBaseColor() const;
	void LampFlashing(uint32 FlashCount);
	void LampFlashing();

	void ChangeIntensity(int32 IntensityChangeDir);

protected:
	virtual void BeginPlay() override;
	
private:
	void ChangeIntensity(int32 IntensityChangeDir, float TargetLightIntensity);
	
	void SetupDefaults();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> LampMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(ToolTip = "Set if mesh doesn't have own material"))
	TObjectPtr<UMaterialInstance> LightMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EZoneColor Color = EZoneColor::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxLightIntensityInCommonColorZone = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxLightIntensityInActivationColorZone = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float IntensityIncreaseFrequency = 0.05f;
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;
	bool bIsLampFlashing = false;
	int32 LampMode = 0;

	FStopFlashingTimer OnStopFlashingTimer;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lamp")
	float CurrentEmissiveIntensity = 0.f;
	
private:
	UPROPERTY()
	TObjectPtr<UWorld> World;
	UPROPERTY()
	FTimerHandle IntensityColorTimer;
	UPROPERTY()
	FTimerHandle FlashingTimer;
	uint32 FlashCounter = 0;
	bool bLampFlashState = true;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FG2ILampEmissiveInfo EmissiveInfo;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true, ToolTip = "Set if needed to override material color"))
	FLinearColor BaseColor = FLinearColor::Transparent;
};

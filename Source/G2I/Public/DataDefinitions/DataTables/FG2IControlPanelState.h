#pragma once

#include "G2ISliderLampComponent.h"
#include "FG2IControlPanelState.generated.h"

USTRUCT(BlueprintType)
struct FG2IControlPanelLampInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Lamps)
	bool bIsChanged = true;

	UPROPERTY(EditAnywhere, Category = Color)
	FG2ILampEmissiveInfo EmissiveInfo;

	UPROPERTY(EditAnywhere, Category = Flashing)
	FG2IFlashingInfo FlashingInfo;
};

/**
 * Table row describing the state of the control panel
 */
USTRUCT(BlueprintType)
struct G2I_API FG2IControlPanelState : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Lamps)
	TArray<FG2IControlPanelLampInfo> Lamps;
};

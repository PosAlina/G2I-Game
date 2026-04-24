#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "G2IOptionsParameters.generated.h"

/**
 * Options Defaults
 */
UCLASS()
class G2I_API UG2IOptionsParameters : public UDataAsset
{
	GENERATED_BODY()
	
public:

	// ==================== KEYBOARD AND MOUSE ====================
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Keyboard and mouse", meta = (ClampMin = "0.0"))
	float MouseSensitive = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Keyboard and mouse")
	bool bIsInvertedCameraHorizontalRotation = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Keyboard and mouse")
	bool bIsInvertedCameraVerticalRotation = false;

	// ==================== GRAPHICS ====================
	// ==================== SCREEN ====================
	// ==================== SOUNDS ====================
};

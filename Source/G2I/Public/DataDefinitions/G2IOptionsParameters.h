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
	float MouseSensitive = 1.f;
	bool bIsInvertedCameraHorizontalRotation = false;
	bool bIsInvertedCameraVerticalRotation = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Keyboard and mouse|MouseSensitive", meta = (ClampMin = "0.0"))
	float DefaultMouseSensitive = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Keyboard and mouse|MouseSensitive", meta = (ClampMin = "0.0"))
	float MinMouseSensitive = 0.1f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Keyboard and mouse|MouseSensitive", meta = (ClampMin = "0.0"))
	float MaxMouseSensitive = 5.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Keyboard and mouse|MouseSensitive", meta = (ClampMin = "0.0"))
	float StepMouseSensitive = 0.1f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Keyboard and mouse|Inversion")
	bool bIsDefaultInvertedCameraHorizontalRotation = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Keyboard and mouse|Inversion")
	bool bIsDefaultInvertedCameraVerticalRotation = false;

	// ==================== GRAPHICS ====================
	// ==================== SCREEN ====================
	// ==================== SOUNDS ====================
	
public:
	
	void SetupDefaultParameters();
	
};
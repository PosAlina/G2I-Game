#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "G2ICutScenesParameters.generated.h"

/**
 * Parameters for setup cutscenes
 */
UCLASS()
class G2I_API UG2ICutScenesParameters : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, Category = Skip)
	float SpeedSkip = 1.f;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = Skip)
	bool bIsDebugOn = true;
#endif
};

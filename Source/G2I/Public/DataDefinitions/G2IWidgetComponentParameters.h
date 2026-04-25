#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "G2IWidgetComponentParameters.generated.h"

/**
 * Parameters of all world widget's component
 */
UCLASS()
class G2I_API UG2IWidgetComponentParameters : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, Category = "Scale on distance", meta = (ClampMin = "1.0"))
	double MinDistanceForChangedScale = 100.;
	
	UPROPERTY(EditAnywhere, Category = "Scale on distance", meta = (ClampMin = "1.0"))
	double MaxDistanceForChangedScale = 3000.;
	
	UPROPERTY(EditAnywhere, Category = "Scale on distance", meta = (ClampMin = "0.005", ClampMax = "0.1"))
	double ScaleForMaxDistance = .005;
	
	UPROPERTY(EditAnywhere, Category = "Scale on distance", meta = (ClampMin = "0.005", ClampMax = "0.1"))
	double ScaleForMinDistance = .1;

};

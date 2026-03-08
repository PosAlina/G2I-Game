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

	UPROPERTY(EditAnywhere)
	FVector2D KeyWidgetDefaultSize = FVector2D(100.f, 100.f);

};

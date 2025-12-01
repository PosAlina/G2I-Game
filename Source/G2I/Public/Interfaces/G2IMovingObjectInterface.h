#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2IMovingObjectInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UG2IMovingObjectInterface : public UInterface
{
	GENERATED_BODY()
};

class G2I_API IG2IMovingObjectInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Data")
	float GetSpeedChange();
};
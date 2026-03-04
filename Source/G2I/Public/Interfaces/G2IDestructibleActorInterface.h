#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2IDestructibleActorInterface.generated.h"

UINTERFACE(MinimalAPI)
class UG2IDestructibleActorInterface : public UInterface
{
	GENERATED_BODY()
};

class G2I_API IG2IDestructibleActorInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void DestroySelf();
};

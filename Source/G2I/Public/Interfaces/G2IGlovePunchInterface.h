

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2IGlovePunchInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UG2IGlovePunchInterface : public UInterface
{
	GENERATED_BODY()
};

class G2I_API IG2IGlovePunchInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void GlovePunchActivation();
};

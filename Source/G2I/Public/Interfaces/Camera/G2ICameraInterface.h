#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2ICameraInterface.generated.h"

class UCameraComponent;

UINTERFACE()
class UG2ICameraInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * This interface determines common camera type
 */
class G2I_API IG2ICameraInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UCameraComponent *GetCameraComponent() const;
	
};

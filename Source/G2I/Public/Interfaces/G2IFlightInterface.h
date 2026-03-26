#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2IFlightInterface.generated.h"

class UCharacterMovementComponent;

UINTERFACE(MinimalAPI)
class UG2IFlightInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * This interface includes methods that are called for flight
 */
class G2I_API IG2IFlightInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Actions)
	void Fly(int Direction);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Actions)
	void StopFly();
};

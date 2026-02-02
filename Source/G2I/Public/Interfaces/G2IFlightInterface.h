

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2IFlightInterface.generated.h"

class UCharacterMovementComponent;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UG2IFlightInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class G2I_API IG2IFlightInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Actions)
	void Fly(UActorComponent* MovementComponent, int Direction);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Actions)
	void StopFly(UActorComponent* MovementComponent);
};

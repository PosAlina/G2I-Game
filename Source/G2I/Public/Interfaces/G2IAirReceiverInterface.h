#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2IAirReceiverInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UG2IAirReceiverInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface that every actor that wants to recieve air from pipes needs to implement.
 */
class G2I_API IG2IAirReceiverInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pipes")
	void ReceiveAir(AActor* Sender, bool bAirPassed);
};

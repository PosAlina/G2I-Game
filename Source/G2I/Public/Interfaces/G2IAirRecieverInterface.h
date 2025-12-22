#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2IAirRecieverInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UG2IAirRecieverInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface that every actor that wants to recieve air from pipes needs to implement.
 */
class G2I_API IG2IAirRecieverInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pipes")
	void RecieveAir(AActor* Sender, bool bAirPassed);
};

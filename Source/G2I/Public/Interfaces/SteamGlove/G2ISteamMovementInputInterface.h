#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2ISteamMovementInputInterface.generated.h"

UINTERFACE()
class UG2ISteamMovementInputInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * This interface includes methods that are called for steam movement from input handlers.
 */
class G2I_API IG2ISteamMovementInputInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Actions)
	void SteamJumpAction();
	
};

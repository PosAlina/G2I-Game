#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2ICharacterInterface.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPossessedDelegate, AController *, NewController);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUnPossessedDelegate);

UINTERFACE()
class UG2ICharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for delegates of standard character functions
 * that are not visible to the actor components of this character
 */
class G2I_API IG2ICharacterInterface
{
	GENERATED_BODY()

public:

	UFUNCTION()
	virtual FPossessedDelegate& GetPossessedDelegate() = 0;

	UFUNCTION()
	virtual FUnPossessedDelegate& GetUnPossessedDelegate() = 0;
	
};

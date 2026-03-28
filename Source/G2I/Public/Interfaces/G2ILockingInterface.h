#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2ILockingInterface.generated.h"

UINTERFACE()
class UG2ILockingInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for objects that can be locked / unlocked
 */
class G2I_API IG2ILockingInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Lock")
	void SetIsLocked(bool bIsNewLocked);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Lock")
	bool IsLocked();
};

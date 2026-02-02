

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2ICatchableObjectInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UG2ICatchableObjectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class G2I_API IG2ICatchableObjectInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactions")
	void AttachToActor(AActor* Attacher);
};

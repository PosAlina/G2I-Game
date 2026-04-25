#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2IUpdateOptions.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UG2IUpdateOptions : public UInterface
{
	GENERATED_BODY()
};

/**
 * Update options for certain topic
 */
class G2I_API IG2IUpdateOptions
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ApplyOptions();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void CancelUnAppliedOptions();
};

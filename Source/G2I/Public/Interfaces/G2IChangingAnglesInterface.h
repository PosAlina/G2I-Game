#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2IChangingAnglesInterface.generated.h"

UINTERFACE()
class UG2IChangingAnglesInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for puzzles with changing angles
 */
class G2I_API IG2IChangingAnglesInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ChangeAngles(const TArray<float>& AngleDeltas, bool bIsOn);
};

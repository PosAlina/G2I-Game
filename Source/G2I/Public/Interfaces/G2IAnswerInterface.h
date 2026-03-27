#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2IAnswerInterface.generated.h"

UINTERFACE()
class UG2IAnswerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for puzzles required answers
 */
class G2I_API IG2IAnswerInterface
{
	GENERATED_BODY()

public:

	// TODO: Need rename or description of this function
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Answer(bool bIsRight);
};

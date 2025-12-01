// 

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2IMovementInputInterface.generated.h"

UINTERFACE()
class UG2IMovementInputInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * This interface includes methods that are called for movement actions from input handlers.
 */
class G2I_API IG2IMovementInputInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Actions)
	void MoveAction(const float Right, const float Forward, const FRotator Rotation);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Actions)
	void JumpAction();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Actions)
	void StopJumpingAction();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Actions)
	void ToggleCrouchAction();
	
};

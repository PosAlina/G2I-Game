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

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Disablers)
	void DisableJump();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Disablers)
	void DisableMove();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Disablers)
	void DisableCrouch();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Disablers)
	void DisableRotation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Enablers)
	void EnableJump();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Enablers)
	void EnableMove();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Enablers)
	void EnableCrouch();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Enablers)
	void EnableRotation();
};

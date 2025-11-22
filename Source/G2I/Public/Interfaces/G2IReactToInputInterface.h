#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2IReactToInputInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UG2IReactToInputInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * This interface includes methods that are called for character actions from input handlers.
 */
class G2I_API IG2IReactToInputInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Actions)
	void MoveAction(const float Right, const float Forward, const FRotator Rotation);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Actions)
	void LookAction(const float Yaw, const float Pitch);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Actions)
	void MouseLookAction(const float Yaw, const float Pitch);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Actions)
	void JumpAction();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Actions)
	void StopJumpingAction();
};

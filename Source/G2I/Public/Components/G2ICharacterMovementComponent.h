#pragma once

#include "CoreMinimal.h"
#include "G2IMovementInputInterface.h"
#include "G2ICharacterMovementComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class G2I_API UG2ICharacterMovementComponent : public UActorComponent, public IG2IMovementInputInterface
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta=(AllowPrivateAccess))
	bool bCanPassThroughObject = false;

public:

	virtual void OnRegister() override;

	// Interface methods
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void MoveAction_Implementation(const float Right, const float Forward, const FRotator Rotation) override;
	
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void JumpAction_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Disablers)
	void DisableJump_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Disablers)
	void DisableMove_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Disablers)
	void DisableCrouch_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Disablers)
	void DisableRotation_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Enablers)
	void EnableJump_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Enablers)
	void EnableMove_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Enablers)
	void EnableCrouch_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = Enablers)
	void EnableRotation_Implementation();

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void StopJumpingAction_Implementation() override;

	// Getters
	bool CanPassThroughObject() const { return bCanPassThroughObject; }

	// Setters
	void SetCanPassThroughObject(bool Value);

};

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

	virtual void PostInitProperties() override;

	// Interface methods
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void MoveAction_Implementation(const float Right, const float Forward, const FRotator Rotation) override;
	
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void JumpAction_Implementation() override;

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void StopJumpingAction_Implementation() override;

	// Getters
	bool CanPassThroughObject() const { return bCanPassThroughObject; }

	// Setters
	void SetCanPassThroughObject(bool Value);
};

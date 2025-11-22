#pragma once

#include "CoreMinimal.h"
#include "G2IMovementInputInterface.h"
#include "G2IMovementComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class G2I_API UG2IMovementComponent : public UActorComponent, public IG2IMovementInputInterface
{
	GENERATED_BODY()

public:

	UG2IMovementComponent();
	
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void MoveAction_Implementation(const float Right, const float Forward, const FRotator Rotation) override;
	
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void JumpAction_Implementation() override;

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void StopJumpingAction_Implementation() override;

};

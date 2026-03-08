

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/G2IActivationInterface.h"
#include "G2IVerticalMovementComponent.generated.h"

class UActorComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveDownFinished);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class G2I_API UG2IVerticalMovementComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	float FindTargetLocation();
	
	void UpdateMovement();

	void StopMovement();

	void MoveUp();
private:
	FTimerHandle MovementTimer;
	FVector StartLocation = FVector::ZeroVector;;
	FVector TargetLocation = FVector::ZeroVector;;
	bool bIsMoving = false;
	bool bIsMovingUp = false;

	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> TargetMeshComponent;
	UPROPERTY()
	TObjectPtr<UWorld> World;
	UPROPERTY()
	TObjectPtr<AActor> Owner;

public:	
	FOnMoveDownFinished OnMoveDownFinished;

	UPROPERTY(EditAnywhere, Category = "Data|Movement")
	float MovingVerticallySpeed = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Data|Movement")
	float MovingDownDistance = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Data|Movement")
	float MovingVerticallyTimerInterval = 0.016f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float ReturnDelay = 2.0f;


	UFUNCTION(BlueprintCallable, Category = "Movement")
	void MoveDown();

	virtual void BeginPlay() override;
};

#pragma once

#include "CoreMinimal.h"
#include "G2IMovementInputInterface.h"
#include "Components/ActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "G2ICharacterMovementComponent.generated.h"

class UCharacterMovementComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class G2I_API UG2ICharacterMovementComponent : public UCharacterMovementComponent, public IG2IMovementInputInterface
{
	GENERATED_BODY()

private:

	UPROPERTY()
	TObjectPtr<ACharacter> Owner;

	UPROPERTY()
	TObjectPtr<UWorld> World;
	
protected:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJumpingDelegate);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta=(AllowPrivateAccess))
	bool bCanPassThroughObject = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess))
	float StandartMaxWalkSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crouch)
	bool bIsCrouchState = false;

public:
	

	UPROPERTY(BlueprintAssignable)
	FJumpingDelegate OnJumpDelegate;

protected:
	
	UG2ICharacterMovementComponent();

	virtual void BeginPlay() override;

	virtual void OnRegister() override;

public:

	// Interface methods
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void MoveAction_Implementation(const float Right, const float Forward, const FRotator Rotation) override;
	
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void JumpAction_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void StopJumpingAction_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "Input")
	void ImpulseJumpAction_Implementation(float ImpulseStrength);

	UFUNCTION(BlueprintCallable, Category = State)
	virtual bool IsInAir_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = Togglers)
	void ToggleJump();

	UFUNCTION(BlueprintCallable, Category = Togglers)
	void ToggleMove();

	UFUNCTION(BlueprintCallable, Category = Togglers)
	void ToggleCrouch();

	UFUNCTION(BlueprintCallable, Category = Togglers)
	void ToggleRotation();

	UFUNCTION(BlueprintCallable, Category = Togglers)
	void ToggleSlow(float NewSpeed);
	
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void ToggleCrouchAction_Implementation() override;

	// Getters
	bool CanPassThroughObject() const { return bCanPassThroughObject; }

	// Setters
	void SetCanPassThroughObject(bool Value);
	
	// Handlers for delegate
	UFUNCTION()
	void HandleMovingInteraction(float SpeedChange);

	bool CanUncrouch() const;

protected:

	void BindingToDelegates();
	
	UFUNCTION()
	void PossessedByNewController(APawn *ChangedPawn);

private:

	void PreInitializationDefaults();
};

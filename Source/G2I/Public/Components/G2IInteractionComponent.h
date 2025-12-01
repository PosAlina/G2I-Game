#pragma once

#include "CoreMinimal.h"
#include "G2IInteractionInputInterface.h"
#include "Components/ActorComponent.h"
#include "G2IInteractionComponent.generated.h"

class UBoxComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class G2I_API UG2IInteractionComponent : public UActorComponent, public IG2IInteractionInputInterface
{
	GENERATED_BODY()

private:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMovingInteractionDelegate, float, SpeedChange);

	/** Interaction Sphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* InteractionBox;

	void BindingToDelegates();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data", meta = (AllowPrivateAccess = "true"))
	bool bCanInteract = true;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	float InteractionBoxLength = 100.f;

	UG2IInteractionComponent();
	

	/** Returns InteractionSphere subobject **/
	FORCEINLINE class UBoxComponent* GetInteractionSphere() const { return InteractionBox; }

	virtual void BeginPlay() override;
	virtual void OnRegister() override;
	
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void InteractAction_Implementation(const FName& Tag) override;

	UPROPERTY(BlueprintAssignable)
	FMovingInteractionDelegate OnMovingInteractingDelegate;

	// Handlers for delegate
	UFUNCTION()
	void HandleJumping();

	UFUNCTION()
	void HandleLanded(const FHitResult& Hit);
};
#pragma once

#include "CoreMinimal.h"
#include "G2IInteractionInputInterface.h"
#include "G2IUIManager.h"
#include "Components/ActorComponent.h"
#include "G2IInteractionComponent.generated.h"

class UInputAction;
class UBoxComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class G2I_API UG2IInteractionComponent : public UActorComponent, public IG2IInteractionInputInterface
{
	GENERATED_BODY()

private:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMovingInteractionDelegate, float, SpeedChange);

	/** Interaction Sphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> InteractionBox;

	void BindingToDelegates();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data", meta = (AllowPrivateAccess = "true"))
	bool bCanInteract = true;

	UPROPERTY()
	TObjectPtr<UG2IUIManager> UIManager;

	UPROPERTY()
	TObjectPtr<ACharacter> Owner;

	UPROPERTY()
	TObjectPtr<AG2IPlayerController> PlayerController;

	UPROPERTY()
	TMap<TObjectPtr<UInputAction>, FName> TagOfInteractionActions;

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

protected:

	UFUNCTION()
	void OnInteractionBoxBeginOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnInteractionBoxEndOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex);

private:

	void SetupDefaults();

	void OpenKeyHintByActor(AActor* OtherActor);
	
	void CloseKeyHintByActor(AActor* OtherActor);

	void SetTagOfInteractionActions();
};
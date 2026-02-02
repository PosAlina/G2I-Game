#pragma once

#include "CoreMinimal.h"
#include "G2IActivationInterface.h"
#include "Components/ActorComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "G2IAIMovementComponent.generated.h"

class AG2IPlayerController;
class AAIController;

/**
 * Component that describes the movement of a character controlled by AI Controller
 */
UCLASS(ClassGroup=(AINavigation), meta=(BlueprintSpawnableComponent))
class G2I_API UG2IAIMovementComponent : public UActorComponent, public IG2IActivationInterface
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UNavigationQueryFilter> FilterClass;
    	
private:

	UPROPERTY()
	TObjectPtr<AAIController> OwnerAIController;

	UPROPERTY()
	TObjectPtr<AG2IPlayerController> PlayerController;

	bool bIsMoving = false;

	bool bCanMove = true;

	bool bActiveComponent = true;
	
public:

	UG2IAIMovementComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void MoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	UFUNCTION()
	void SetCanMove(bool bNewCanMove);

	void UpdateNewAIPawn(APawn *NewAIPawn);

	UFUNCTION(BlueprintCallable, Category = "Activation")
	virtual void Activate_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "Activation")
	virtual void Deactivate_Implementation() override;
	
private:

	void SetupDefaults();
	void SetupDefaultsAIOwnerController();
	void SetupDefaultsPlayerController();
	void BindDelegates();

	bool ShouldMove();
	
	void UpdateTickMovement();

	void StopMovement();
	
};

#pragma once

#include "CoreMinimal.h"
#include "G2IInteractionInputInterface.h"
#include "G2IInteractionComponent.generated.h"

class USphereComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class G2I_API UG2IInteractionComponent : public UActorComponent, public IG2IInteractionInputInterface
{
	GENERATED_BODY()

private:
	/** Interaction Sphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* InteractionSphere;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	float InteractionSphereRadius = 200.f;

	UG2IInteractionComponent();
	

	/** Returns InteractionSphere subobject **/
	FORCEINLINE class USphereComponent* GetInteractionSphere() const { return InteractionSphere; }

	virtual void OnRegister() override;
	
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void InteractAction_Implementation(const FName& Tag) override;
};
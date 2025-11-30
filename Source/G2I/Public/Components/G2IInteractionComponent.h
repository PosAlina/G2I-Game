#pragma once

#include "CoreMinimal.h"
#include "G2IInteractionInputInterface.h"
#include "G2IInteractionComponent.generated.h"

class UBoxComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class G2I_API UG2IInteractionComponent : public UActorComponent, public IG2IInteractionInputInterface
{
	GENERATED_BODY()

private:
	/** Interaction Sphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* InteractionBox;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	float InteractionBoxLength = 100.f;

	UG2IInteractionComponent();
	

	/** Returns InteractionSphere subobject **/
	FORCEINLINE class UBoxComponent* GetInteractionSphere() const { return InteractionBox; }

	virtual void OnRegister() override;
	
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void InteractAction_Implementation(const FName& Tag) override;
};
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "G2IActivationWithOrderComponent.generated.h"

DECLARE_DELEGATE_ThreeParams(FOnActivatedDelegate, AActor* /* ActivatedActor */, bool /* bReactivation */, UG2IActivationWithOrderComponent* /* ActivationComponent */ );

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class G2I_API UG2IActivationWithOrderComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	FOnActivatedDelegate OnActivatedDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activation with order",
		meta = (Tooltip="Can it be activated twice (e.g. deactivated, calcelled, etc.)?"))
	bool bCanBeReactivated = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Activation with order")
	bool bActivated = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Activation with order")
	TArray<int32> ActivatedIndexes;

public:	
	UG2IActivationWithOrderComponent() = default;

	UFUNCTION(BlueprintCallable, Category = "Activation with order")
	void Activated();

	UFUNCTION(BlueprintCallable, Category = "Activation with order")
	void Accepted(int32 AtIndex);

	UFUNCTION(BlueprintCallable, Category = "Activation with order")
	void Declined();

	UFUNCTION(BlueprintCallable, Category = "Activation with order")
	void Restored();
};

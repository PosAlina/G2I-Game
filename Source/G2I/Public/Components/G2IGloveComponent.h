

#pragma once

#include "CoreMinimal.h"
#include "G2IGadgetInterface.h"
#include "Components/ActorComponent.h"
#include "G2IGloveComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGadgetActivationEventSignature);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class G2I_API UG2IGloveComponent : public UActorComponent, public IG2IGadgetInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float PunchArea = 50.f;

private:
	bool bIsGloveEnabled = true;
	TArray<AActor*> ActorsToDestroy;
	APlayerController* PC;
	
public:	
	UG2IGloveComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "GlovePunch")
	virtual void ActivatePunch();

	UFUNCTION(BlueprintCallable, Category = "GlovePunch")
	TArray<AActor*> GetActorsToDestroy();

	UFUNCTION(BlueprintCallable, Category = "GlovePunch")
	bool IsGloveEnabled();

	UFUNCTION(BlueprintCallable)
	void InputControl(bool isInputEnabled);

	UFUNCTION(BlueprintCallable)
	virtual void GadgetActivation_Implementation() override;

	UPROPERTY(BlueprintAssignable, Category = "GadgetPunch")
	FOnGadgetActivationEventSignature OnGadgetActivationEvent;
	
protected:
	virtual void BeginPlay() override;
};

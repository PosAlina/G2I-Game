#pragma once

#include "CoreMinimal.h"
#include "G2IGadgetInterface.h"
#include "Components/ActorComponent.h"
#include "G2IGlovePunchComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGadgetActivationEventSignature);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class G2I_API UG2IGlovePunchComponent : public UActorComponent, public IG2IGadgetInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "GlovePunch")
	float PunchArea = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GlovePunch")
	bool bIsNeedToControlInput = false;
	UPROPERTY(EditAnywhere, Category = "GlovePunch")
	FName PunchBoneName = "hand_r";
	UPROPERTY(EditAnywhere, Category = "GlovePunch")
	TObjectPtr<UAnimMontage> GlovePunchAnimMontage;
	
private:
	UPROPERTY()
	TArray<AActor*> ActorsToDestroy;
	
public:	
	UG2IGlovePunchComponent();
	
	void ActivatePunch();
	TArray<AActor*> GetActorsToDestroy();
	void ClearActorsToDestroy();
	virtual void GadgetActivation_Implementation() override;
};

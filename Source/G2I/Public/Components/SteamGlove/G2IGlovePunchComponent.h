#pragma once

#include "CoreMinimal.h"
#include "G2IGlovePunchInterface.h"
#include "Components/ActorComponent.h"
#include "G2IGlovePunchComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class G2I_API UG2IGlovePunchComponent : public UActorComponent, public IG2IGlovePunchInterface
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
	UPROPERTY(EditAnywhere, Category = "GlovePunch")
	FVector DefaultPunchLocation;

	FVector CurrentDefaultPunchLocation;
	
private:
	UPROPERTY()
	TArray<AActor*> ActorsToDestroy;
	
public:
	
	void ActivatePunch();
	TArray<AActor*> GetActorsToDestroy();
	void ClearActorsToDestroy();
	virtual void GlovePunchActivation_Implementation() override;
};

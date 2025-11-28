

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "G2IGloveComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class G2I_API UG2IGloveComponent : public UActorComponent
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
	
protected:
	virtual void BeginPlay() override;
};

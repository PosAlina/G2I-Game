#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/G2IPipesBoxComponent.h"
#include "G2ISendToPipesComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class G2I_API UG2ISendToPipesComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	bool bHasAir = true;

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<UG2IPipesBoxComponent> PipesBoxComponent;

private:
	UPROPERTY(VisibleInstanceOnly)
	TArray<TObjectPtr<AActor>> ActorsToSendAirTo;

public:
	UG2ISendToPipesComponent();

	UFUNCTION()
	void SendAir() const;

	UFUNCTION(BlueprintCallable)
	void SetHasAir(const bool bNewHasAir);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPipesBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPipesBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void ForceOverlaps();
};

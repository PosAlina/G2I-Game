
#pragma once

#include "G2I.h"
#include "GameFramework/Actor.h"
#include "Components/G2IPipesBoxComponent.h"
#include "Interfaces/G2IAirReciever.h"
#include "G2IAirTab.generated.h"

UCLASS()
class G2I_API AG2IAirTab : public AActor, public IG2IAirReciever
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AG2IAirTab();

	void OnConstruction(const FTransform& Transform) override;

	void RecieveAir_Implementation(AActor* sender, bool bAirPassed) override;

	UFUNCTION(BlueprintCallable)
	bool CheckIfEnoughAir();

	UFUNCTION(BlueprintCallable)
	bool GetActivated() const;

	UFUNCTION(BlueprintCallable)
	void ChangeActivated(bool newActivated);

	UFUNCTION(BlueprintCallable)
	void ActivateActors();

	UFUNCTION(BlueprintCallable)
	void DeactivateActors();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tab")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tab")
	int NumOfPipesNeeded;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tab")
	TArray<TObjectPtr<AActor>> ActorsToActivate;

	TObjectPtr<UG2IPipesBoxComponent> BoxComponent;

private:
	UPROPERTY(VisibleAnywhere)
	bool bActivated;

	UPROPERTY(EditAnywhere, Category = "Tab")
	FVector BoxExtents = FVector(15.f);

	TMap<TObjectPtr<AActor>, bool> AirSendersMap;
};

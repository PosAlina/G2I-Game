
#pragma once

#include "G2I.h"
#include "GameFramework/Actor.h"
#include "Components/G2IPipesBoxComponent.h"
#include "Interfaces/G2IAirReciever.h"
#include "G2IAirTab.generated.h"

UCLASS(Blueprintable, Placeable)
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tab")
	int NumOfPipesNeeded = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tab")
	TArray<TObjectPtr<AActor>> ActorsToActivate;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UG2IPipesBoxComponent> BoxComponent;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Tab")
	bool bActivated = false;

	UPROPERTY(EditAnywhere, Category = "Tab")
	FVector BoxExtents = FVector(15.f);

	UPROPERTY()
	TMap<TObjectPtr<AActor>, bool> AirSendersMap;
};

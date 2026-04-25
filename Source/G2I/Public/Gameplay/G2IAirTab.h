#pragma once

#include "GameFramework/Actor.h"
#include "Components/G2IPipesBoxComponent.h"
#include "Interfaces/G2IAirReceiverInterface.h"
#include "G2IAirTab.generated.h"

class UG2ILauncherComponent;

UCLASS(Blueprintable, Placeable)
class G2I_API AG2IAirTab : public AActor, public IG2IAirReceiverInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AG2IAirTab();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void ReceiveAir_Implementation(AActor* Sender, bool bAirPassed) override;

	UFUNCTION(BlueprintCallable)
	bool CheckIfEnoughAir() const;

	UFUNCTION(BlueprintCallable)
	bool GetActivated() const;

	UFUNCTION(BlueprintCallable)
	virtual void ChangeActivated(const bool bNewActivated);

	UFUNCTION(BlueprintCallable)
	void ActivateActors();

	UFUNCTION(BlueprintCallable)
	void DeactivateActors();

protected:
	virtual void BeginPlay() override;

public:
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UG2ILauncherComponent> LauncherComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 NumOfPipesNeeded = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<AActor>> ActorsToActivate;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UG2IPipesBoxComponent> BoxComponent;

protected:
	UPROPERTY(VisibleAnywhere)
	bool bActivated = false;

	UPROPERTY(EditAnywhere)
	FVector BoxExtents = FVector(50.f);

	UPROPERTY()
	TMap<TObjectPtr<AActor>, bool> AirSendersMap;
};

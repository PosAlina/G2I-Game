

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "G2ISpawnLight.generated.h"

class USpotLightComponent;
class UG2IInventoryComponent;

UCLASS()
class G2I_API AG2ISpawnLight : public AActor
{
	GENERATED_BODY()
	
public:	
	AG2ISpawnLight();

protected:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USceneComponent> Root;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> Mesh;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USpotLightComponent> SpotLight;

    UPROPERTY(EditAnywhere, Category = "Interaction")
    FName RequiredItemID;
public:	

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void OnLight(AActor* Interactor);

};

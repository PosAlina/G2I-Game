#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/G2IActivationInterface.h"
#include "G2IActivatableLamp.generated.h"

class UPointLightComponent;
class UStaticMeshComponent;
class UMaterialInterface;

UCLASS()
class G2I_API AG2IActivatableLamp : public AActor, public IG2IActivationInterface
{
	GENERATED_BODY()
	
public:	
	AG2IActivatableLamp();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> LampBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Lamp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPointLightComponent> PointLight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lamp")
	TObjectPtr<UMaterialInterface> ActivatedMaterial;

	virtual void Activate_Implementation() override;
};

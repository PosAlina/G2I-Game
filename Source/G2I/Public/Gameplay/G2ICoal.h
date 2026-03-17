#pragma once

#include "CoreMinimal.h"
#include "Interfaces/G2IActivationInterface.h"
#include "GameFramework/Actor.h"
#include "G2ICoal.generated.h"

class UG2IDestructibleComponent;

UCLASS(Blueprintable, Placeable)
class G2I_API AG2ICoal : public AActor, public IG2IActivationInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRootComponent;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data|Components")
	TObjectPtr<UG2IDestructibleComponent> DestructComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data|Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	virtual void Activate_Implementation() override;
	virtual void Deactivate_Implementation() override;

	AG2ICoal();
};

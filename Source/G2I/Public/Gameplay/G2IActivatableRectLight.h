#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/G2IActivationInterface.h"
#include "Components/RectLightComponent.h"
#include "G2IActivatableRectLight.generated.h"

class URectLightComponent;

UCLASS()
class G2I_API AG2IActivatableRectLight : public AActor, public IG2IActivationInterface
{
	GENERATED_BODY()

public:
	AG2IActivatableRectLight();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URectLightComponent> RectLight;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Activate_Implementation() override;
};
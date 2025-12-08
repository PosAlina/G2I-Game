#pragma once

#include "CoreMinimal.h"
#include "CineCameraComponent.h"
#include "GameFramework/Actor.h"
#include "G2IFixedCameraActor.generated.h"

class UBoxComponent;

UCLASS()
class G2I_API AG2IFixedCameraActor : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UBoxComponent> FixedCameraZoneTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCineCameraComponent> FixedCamera;

protected:

	AG2IFixedCameraActor();

public:
	
	UBoxComponent *GetFixedCameraZoneTrigger();

	UCameraComponent *GetFixedCamera();
	
private:

	void InitializeFixedCameraZoneTrigger();

	void InitializeFixedCamera();
	
};

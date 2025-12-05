#pragma once

#include "CoreMinimal.h"
#include "CineCameraActor.h"
#include "Components/Camera/G2IFixedCamerasComponent.h"
#include "GameFramework/Actor.h"
#include "G2IFixedCameraActor.generated.h"

class ACineCameraActor;
class ATriggerBox;

UCLASS()
class G2I_API AG2IFixedCameraActor : public ACineCameraActor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<ATriggerBox> FixedCameraZoneTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<ACineCameraActor> FixedCamera;

protected:

	virtual void BeginPlay() override;
	
};

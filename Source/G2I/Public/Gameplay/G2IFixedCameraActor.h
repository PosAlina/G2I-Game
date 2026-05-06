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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	bool bShouldFollow = true;

	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

protected:

	AG2IFixedCameraActor();

	virtual void BeginPlay() override;

public:
	
	UBoxComponent *GetFixedCameraZoneTrigger();

	UCameraComponent *GetFixedCamera();

	void StartFollow();

	void StopFollow();

	virtual void Tick(float DeltaTime) override;
	
private:

	void InitializeFixedCameraZoneTrigger();

	void InitializeFixedCamera();

};

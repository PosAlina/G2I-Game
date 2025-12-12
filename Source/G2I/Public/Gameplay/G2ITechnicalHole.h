
#pragma once

#include "G2I.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Interfaces/G2IInteractiveObjectInterface.h"
#include "G2ITechnicalHole.generated.h"

UCLASS()
class G2I_API AG2ITechnicalHole : public AActor, public IG2IInteractiveObjectInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AG2ITechnicalHole();

	// Only Engineer can interact
	bool CanInteract_Implementation(const ACharacter* Interactor) override;

	// Basic implementation works only for pipes,
	// override it if you want to extend the execution
	void Interact_Implementation(const ACharacter* Interactor) override;

	UFUNCTION()
	void StartSendingAir();

	UFUNCTION()
	void StopSendingAir();

public:
	UPROPERTY()
	int32 PointIndex;

	UPROPERTY(EditAnywhere, Category="Hole",
		meta=(ToolTip="Time (in seconds) that takes for air to move through the pipes."))
	float TimeToSendAir = 5.f;

private:
	UPROPERTY()
	FTimerHandle TimerHandle;

	TObjectPtr<USceneComponent> RootSceneComponent;

	TObjectPtr<UBoxComponent> BoxComponent;
};

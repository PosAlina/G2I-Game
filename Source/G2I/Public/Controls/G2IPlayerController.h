#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "G2IPlayerController.generated.h"

class UInputMappingContext;

/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(abstract)
class AG2IPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	/** TODO: Refactor Input */
	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> InputMappingContexts;

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	// Input Handlers
	void SetupPlayerInputBindings();
};

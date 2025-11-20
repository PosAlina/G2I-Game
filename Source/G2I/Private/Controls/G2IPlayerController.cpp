#include "G2IPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "G2ICharacterEngineer.h"
#include "G2IHUDActionsHandler.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"

void AG2IPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AG2IPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : InputMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}
		}
	}

}
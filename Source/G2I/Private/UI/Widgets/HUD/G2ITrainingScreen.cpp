#include "HUD/G2ITrainingScreen.h"

#include "G2I.h"
#include "G2ICharacterDaughter.h"
#include "G2ICharacterEngineer.h"
#include "G2IPlayerController.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"

void UG2ITrainingScreen::InitializeAfterManagerLoading()
{
	Super::InitializeAfterManagerLoading();

	BindDelegates();
	
	if (!ensure(CharactersDescriptionsSwitcher))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Character Description Switcher"), *GetName());
		return;
	}
	CharactersDescriptionsSwitcher->AddChild(EngineerDescription);
	CharactersDescriptionsSwitcher->AddChild(DaughterDescription);
}

void UG2ITrainingScreen::BindDelegates()
{
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*AG2IPlayerController::StaticClass()->GetName());
		return;
	}
	PlayerController->OnPossessPawnDelegate.AddDynamic(this, &ThisClass::OnSwitchCharactersDescription);
}

void UG2ITrainingScreen::OnSwitchCharactersDescription(APawn* Pawn)
{
	if (!ensure(Pawn))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: An attempt to switch character description for null pawn"), *GetName());
		return;
	}
	if (!ensure(CharactersDescriptionsSwitcher))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Character Description Switcher"), *GetName());
		return;
	}
	const UClass *PawnClass = Pawn->GetClass();
	if (!ensure(PawnClass))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find class of %s"), *GetName(), *Pawn->GetName());
		return;
	}
	
	if (PawnClass->IsChildOf<AG2ICharacterEngineer>())
	{
		CharactersDescriptionsSwitcher->SetActiveWidget(EngineerDescription);
		return;
	}
	
	if (PawnClass->IsChildOf<AG2ICharacterDaughter>())
	{
		CharactersDescriptionsSwitcher->SetActiveWidget(DaughterDescription);
	}
}

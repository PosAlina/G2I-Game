#include "Gameplay/G2IMovingBySteamAndHandsObject.h"

AG2IMovingBySteamAndHandsObject::AG2IMovingBySteamAndHandsObject()
{
	PrimaryActorTick.bCanEverTick = false;

}

bool CanInteract_Implementation(const ACharacter* Interactor) {

	return PossibleInteractors.Contains(Interactor);
}
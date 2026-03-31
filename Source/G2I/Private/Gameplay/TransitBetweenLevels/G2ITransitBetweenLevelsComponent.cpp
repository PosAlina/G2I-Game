#include "TransitBetweenLevels/G2ITransitBetweenLevelsComponent.h"
#include "G2I.h"
#include "G2IGameInstance.h"

void UG2ITransitBetweenLevelsComponent::BeginPlay()
{
	Super::BeginPlay();

	const UWorld *World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}
	GameInstance = Cast<UG2IGameInstance>(World->GetGameInstance());
	if (!ensure(GameInstance))
	{
		UE_LOG(LogG2I, Error, TEXT("Game Instance doesn't exist in %s"), *GetName());
		return;
	}
}

bool UG2ITransitBetweenLevelsComponent::Transit() const
{
	if (!ensure(GameInstance))
	{
		UE_LOG(LogG2I, Error, TEXT("Game Instance doesn't exist in %s"), *GetName());
		return false;
	}
	switch(TransitType)
	{
	case EG2ITransitBetweenLevelType::TransitNextLevel:
		return GameInstance->LoadNextLevel();
	case EG2ITransitBetweenLevelType::TransitMainMenu:
		return GameInstance->LoadMainMenuLevel();
	case EG2ITransitBetweenLevelType::TransitByName:
		return GameInstance->LoadLevel(TargetLevelName);
	case EG2ITransitBetweenLevelType::TransitByIndex:
		return GameInstance->LoadLevel(TargetLevelIndex);
	}
	return false;
}

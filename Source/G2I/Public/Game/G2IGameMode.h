#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/GameModeBase.h"
#include "G2IGameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(Blueprintable, BlueprintType)
class AG2IGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	
	/** Constructor */
	AG2IGameMode();
};




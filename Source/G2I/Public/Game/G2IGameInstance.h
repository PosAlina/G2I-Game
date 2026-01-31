#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/SavingSystem/G2ISaveGameplayInterface.h"
#include "Interfaces/SavingSystem/G2ISaveSettingsInterface.h"
#include "G2IGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class G2I_API UG2IGameInstance : public UGameInstance
{
	GENERATED_BODY()

	// async & sync
	// one or several slots

public:
	

	// save settings

	// load settings

	// get delegate

public:
	// delegate w/ save game parameter
	// GameSaved

	// delegate w/ save game parameter
	// SettingsSaved

private:
	// SaveGame w/ gameplay info

	// SaveGame w/ settings
};

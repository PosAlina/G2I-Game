#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/SavingSystem/G2ISaveGameplayInterface.h"
#include "Interfaces/SavingSystem/G2ISaveSettingsInterface.h"
#include "G2IGameInstance.generated.h"

class UG2IWidgetsCatalog;

/**
 * Base game instance for G2I game
 */
UCLASS()
class G2I_API UG2IGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TObjectPtr<UG2IWidgetsCatalog> WidgetsCatalog;

	// async & sync
	// one or several slots

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

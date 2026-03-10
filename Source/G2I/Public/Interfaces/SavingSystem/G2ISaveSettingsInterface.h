#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "G2ISaveSettingsInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UG2ISaveSettingsInterface : public UInterface
{
	GENERATED_BODY()
};

/*
 Allows to save, load, sync data with SaveGame object that corresponds to storing settings data
 (such as music volume, key bindings, etc.) and to return delegates of that actions.
 */
class G2I_API IG2ISaveSettingsInterface
{
	GENERATED_BODY()

public:
};

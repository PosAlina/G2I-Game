#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
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

};

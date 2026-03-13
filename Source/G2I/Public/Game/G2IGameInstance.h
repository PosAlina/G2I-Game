#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "G2IGameInstance.generated.h"

class UG2IWidgetComponentParameters;
class UG2IStringTablesCatalog;
class UG2IWidgetsCatalog;

/**
 * Base game instance for G2I game
 */
UCLASS()
class G2I_API UG2IGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere)
	TObjectPtr<UG2IWidgetsCatalog> WidgetsCatalog;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UG2IStringTablesCatalog> StringTablesCatalog;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UG2IWidgetComponentParameters> WidgetComponentsParameters;

public:
	
	UG2IWidgetsCatalog *GetWidgetsCatalog();

	UG2IStringTablesCatalog *GetStringTablesCatalog();

	UG2IWidgetComponentParameters *GetWidgetComponentParameters();
	
};

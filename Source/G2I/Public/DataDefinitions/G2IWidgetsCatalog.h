#pragma once

#include "CoreMinimal.h"
#include "G2IWidgetTypes.h"
#include "Engine/DataAsset.h"
#include "G2IWidgetsCatalog.generated.h"

class UG2IUserWidget;
enum class EG2IWidgetNames : uint8;

USTRUCT(BlueprintType)
struct FG2IWidgetClassesInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<UG2IUserWidget> Class;

	UPROPERTY(EditAnywhere)
	EG2IWidgetTypes Type = EG2IWidgetTypes::UI;
};

/**
 * List of all blueprint widgets
 */
UCLASS()
class G2I_API UG2IWidgetsCatalog : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TMap<EG2IWidgetNames, FG2IWidgetClassesInfo> WidgetClasses;

};

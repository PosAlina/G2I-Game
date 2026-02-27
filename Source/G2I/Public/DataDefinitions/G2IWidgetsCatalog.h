#pragma once

#include "CoreMinimal.h"
#include "G2IWidgetsEnums.h"
#include "Engine/DataAsset.h"
#include "Widgets/G2IPauseWidget.h"
#include "G2IWidgetsCatalog.generated.h"

enum class EG2IUIType : uint8;
enum class EG2IAimType : uint8;
enum class EG2IWidgetNames : uint8;

USTRUCT(BlueprintType)
struct FG2IWidgetClassesInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<UG2IUserWidget> Class;

	UPROPERTY(EditAnywhere)
	EG2IUIType Type = EG2IUIType::HUD;
};

USTRUCT(BlueprintType)
struct FG2IMultiText
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FString> NextStrings;
};

/**
 * All changing information about widgets
 */
UCLASS()
class G2I_API UG2IWidgetsCatalog : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TMap<EG2IWidgetNames, FG2IWidgetClassesInfo> WidgetClasses;

	// =================== AIMING WIDGET ===================
	UPROPERTY(EditAnywhere, Category = Aiming, meta = (ToolTip = "List of images for different aim states"))
	TMap<EG2IAimType, TObjectPtr<UTexture2D>> AimingViewTypes;

	// =================== TRAINING SCREEN WIDGET ===================
	UPROPERTY(EditAnywhere, Category = TrainingScreen, meta = (ToolTip = "List of descriptions of common mechanics", MultiLine = true))
	FG2IMultiText CommonMechanicsDescription;
	
	UPROPERTY(EditAnywhere, Category = TrainingScreen, meta = (ToolTip = "List of descriptions of unique mechanics for characters", MultiLine = true))
	TMap<TSubclassOf<APawn>, FG2IMultiText> CharactersMechanicsDescription;

	// =================== WORLD WIDGET ===================
	UPROPERTY(EditAnywhere, Category = WorldWidget)
	FVector2D KeyWidgetDefaultSize = FVector2D(100.f, 100.f);
};

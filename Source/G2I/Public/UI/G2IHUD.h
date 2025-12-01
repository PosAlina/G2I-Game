#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "G2IHUD.generated.h"

/**
 * 
 */
UCLASS()
class G2I_API AG2IHUD : public AHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitHud();
	
	UFUNCTION(BlueprintCallable)
	void AddWidgetToViewport(UUserWidget* WidgetToAdd);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> InputDescriptionWidgetClass;

protected:
	virtual void BeginPlay() override;
	
private:
	UUserWidget* InputDescriptionWidget;
};

#pragma once

#include "CoreMinimal.h"
#include "G2IWorldHintWidgetComponent.h"
#include "G2IWorldHintKeyWidgetComponent.generated.h"

class UG2IKeyHintWidget;
class UInputAction;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class G2I_API UG2IWorldHintKeyWidgetComponent : public UG2IWorldHintWidgetComponent
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, Category = "Key widget", meta = (AllowPrivateAccess = true, ToolTip = "Input action, if manual input is required"))
	TObjectPtr<UInputAction> OverridenInputAction;
	
public:

	void OpenKeyHint(UInputAction *KeyInput = nullptr);

	void CloseKeyHint();

protected:

	UG2IWorldHintKeyWidgetComponent();

	virtual void BeginPlay() override;
	
};

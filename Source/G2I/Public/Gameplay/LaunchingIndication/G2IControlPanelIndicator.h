#pragma once

#include "CoreMinimal.h"
#include "FG2IControlPanelState.h"
#include "G2ILaunchingActorsIndicator.h"
#include "G2IControlPanelIndicator.generated.h"

UCLASS()
class G2I_API AG2IControlPanelIndicator : public AG2ILaunchingActorsIndicator
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, Category = States)
	TObjectPtr<UDataTable> StatesDataTable;
	
	UPROPERTY()
	TArray<TObjectPtr<UG2ISliderLampComponent>> Lamps;
	
protected:

	virtual void UpdateState_Implementation() override;

	UFUNCTION(BlueprintCallable)
	void InitializeLamps(const TArray<UG2ISliderLampComponent*>& InLamps);

private:

	void SetLampsForCurrentState();

	FG2IControlPanelState *GetControlPanelState(const FName& StateName) const;
	
};
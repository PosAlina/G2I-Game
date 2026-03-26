#include "LaunchingIndication/G2IControlPanelIndicator.h"
#include "FG2IControlPanelState.h"

void AG2IControlPanelIndicator::UpdateState_Implementation()
{
	Super::UpdateState_Implementation();
	
	SetLampsForCurrentState();
}

void AG2IControlPanelIndicator::InitializeLamps(const TArray<UG2ISliderLampComponent*>& InLamps)
{
	Lamps = InLamps;

	SetLampsForCurrentState();
}

void AG2IControlPanelIndicator::SetLampsForCurrentState()
{
	FG2IControlPanelState *SateInfo = GetControlPanelState(GetCurrentState());
	if (!SateInfo)
	{
		// If the indicator state doesn't affect the lamps
		return;
	}

	for (int32 Index = 0; Index < SateInfo->Lamps.Num(); ++Index)
	{
		if (!Lamps.IsValidIndex(Index))
		{
			DebugWarningMessage(GetActorNameOrLabel() + ": Couldn't find " + FString::FromInt(Index) + " lamp");
			break;
		}
		const FG2IControlPanelLampInfo& LampInfo = SateInfo->Lamps[Index];
		if (!LampInfo.bIsChanged)
		{
			continue;
		}
		
		Lamps[Index]->StopTimerToFlashing();
		Lamps[Index]->SetEmissiveColor(LampInfo.EmissiveInfo.Color);
		Lamps[Index]->SetMaxEmissiveIntensity(LampInfo.EmissiveInfo.MaxIntensity);

		if (LampInfo.FlashingInfo.bFlashingIsOn)
		{
			Lamps[Index]->SetTimerToFlashing(LampInfo.FlashingInfo.Rate);
			continue;
		}
		
		if (LampInfo.EmissiveInfo.bIsOn)
		{
			Lamps[Index]->OnLamp();
		}
		else
		{
			Lamps[Index]->OffLamp();
		}
	}
}

FG2IControlPanelState* AG2IControlPanelIndicator::GetControlPanelState(const FName& StateName) const
{
	if (StateName == NAME_None)
	{
		return nullptr;
	}
	FG2IControlPanelState *StateInfo = StatesDataTable->FindRow<FG2IControlPanelState>(StateName,
TEXT("Loading state info for control panel"));
	if (!ensure(StateInfo))
	{
		DebugWarningMessage(GetActorNameOrLabel() + "couldn't find state info for current state with name "
			+ GetCurrentState().ToString());
		return nullptr;
	}
	return StateInfo;
}

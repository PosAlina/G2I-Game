#include "Gameplay/G2IFurnaceDoor.h"
#include "TimerManager.h"
#include "G2I.h"

void AG2IFurnaceDoor::StartTick()
{
	CurrentTicks = 0;
	
	if (MaxTicks <= 0) {
		return;
	}

	const UWorld* World = GetWorld();
	if (!ensure(World)) {
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}

	const float Interval = TotalTime / MaxTicks;

	World->GetTimerManager().SetTimer(
		PushTimerHandle,
		this,
		&AG2IFurnaceDoor::PerformTick,
		Interval,
		true
	);
}

void AG2IFurnaceDoor::PerformTick()
{
	OnPushing_Implementation(Force);

	CurrentTicks++;

	if (CurrentTicks < MaxTicks)
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (!ensure(World)) {
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}

	World->GetTimerManager().ClearTimer(PushTimerHandle);
}

void AG2IFurnaceDoor::Activate_Implementation()
{
	StartTick();
}

void AG2IFurnaceDoor::Deactivate_Implementation() {
	return;
}

#include "LaunchingIndication/G2ILauncherComponent.h"
#include "G2I.h"
#include "G2IWorldHintKeyWidgetComponent.h"

void UG2ILauncherComponent::SetIsLaunched(const bool bNewIsLaunched)
{
	if (bIsLaunched == bNewIsLaunched)
	{
		return;
	}
	bIsLaunched = bNewIsLaunched;
	if (bIsLaunched && bLockAfterLaunch)
	{
		SetIsLocked_Implementation(true);
	}

	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find owner"), *GetName());
		return;
	}
	OnLaunchDelegate.Broadcast(this,Owner, bIsLaunched);
}

bool UG2ILauncherComponent::IsLaunched() const
{
	return bIsLaunched;
}

FLaunchDelegate& UG2ILauncherComponent::GetOnLaunchDelegate()
{
	return OnLaunchDelegate;
}

void UG2ILauncherComponent::SetIsLocked_Implementation(const bool bNewIsLocked)
{
	if (bIsLocked == bNewIsLocked)
	{
		return;
	}
	bIsLocked = bNewIsLocked;

	if (HintKeyComp)
	{
		HintKeyComp->SetIsLocked_Implementation(bNewIsLocked);
	}
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find owner"), *GetName());
		return;
	}
	OnLockedDelegate.Broadcast(this, Owner, bIsLocked);
	DebugLockedMessage();
}

bool UG2ILauncherComponent::IsLocked_Implementation()
{
	return bIsLocked;
}

void UG2ILauncherComponent::SetHintKeyWidget(UG2IWorldHintKeyWidgetComponent* InHintKeyWidget)
{
	if (!ensure(InHintKeyWidget))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Attempt to set null hint widget"), *GetName());
		return;
	}
	HintKeyComp = InHintKeyWidget;
	HintKeyComp->SetIsLocked_Implementation(bIsLocked);
}

FLockedDelegate& UG2ILauncherComponent::GetOnLockedDelegate()
{
	return OnLockedDelegate;
}

void UG2ILauncherComponent::OnRegister()
{
	Super::OnRegister();

	Owner = GetOwner();
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find owner"), *GetName());
		return;
	}
}

void UG2ILauncherComponent::DebugLockedMessage() const
{
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find owner"), *GetName());
		return;
	}
	
	FString DebugMessage = Owner->GetActorNameOrLabel() + " ";
	DebugMessage += bIsLocked ? TEXT("locked") : TEXT("un locked");
	const FColor& DebugColor = bIsLocked ? FColor::Purple : FColor::Green;
	G2I::DebugLogMessage(DebugMessage, DebugColor);
}

void UG2ILauncherComponent::DebugLaunchedMessage() const
{
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find owner"), *GetName());
		return;
	}
	
	FString DebugMessage = Owner->GetActorNameOrLabel() + " ";
	DebugMessage += bIsLaunched ? TEXT("launched") : TEXT("un launched");
	const FColor& DebugColor = bIsLaunched ? FColor::Green : FColor::Purple;
	G2I::DebugLogMessage(DebugMessage, DebugColor);
}
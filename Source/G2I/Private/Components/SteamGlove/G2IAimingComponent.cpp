#include "G2IAimingComponent.h"
#include "G2I.h"
#include "G2IAimingWidget.h"
#include "G2IMechanicUsingAimInterface.h"
#include "G2IPlayerController.h"
#include "G2ISteamShotComponent.h"
#include "Blueprint/UserWidget.h"
#include "G2ICameraControllerComponent.h"
#include "G2ICharacterInterface.h"

UG2IAimingComponent::UG2IAimingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UG2IAimingComponent::BeginPlay()
{
	Super::BeginPlay();

	SetupDefaults();
	BindDelegates();
	ActivateCurrentComponentUsingAim();
}

void UG2IAimingComponent::TickComponent(float DeltaTime, ELevelTick TickType,
										FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsAiming)
	{
		DetectAimLineHitInfo();
	}

	if (bAimViewIsPending)
	{
		PendingAimViewElapsedTime += DeltaTime;
		if (PendingAimViewElapsedTime >= PendingAimViewMaxTime)
		{
			bAimViewIsPending = false;
			PendingAimViewElapsedTime = 0.f;
			AimWidget->SetAimTexture(CurrentAimType);
		}
	}
}

void UG2IAimingComponent::SetupDefaults()
{
	const AActor *Owner = GetOwner();
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner doesn't exist in %s"), *GetName());
	}
	else
	{
		ComponentsUsingAim = Owner->GetComponentsByInterface(UG2IMechanicUsingAimInterface::StaticClass());
	}
	
	World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}

	APlayerController* FirstPlayerController = World->GetFirstPlayerController();
	if (!ensure(FirstPlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Player Controller doesn't exist in %s"), *GetName());
		return;
	}

	PlayerController = Cast<AG2IPlayerController>(FirstPlayerController);
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Player Controller isn't target %s in %s"),
			*AG2IPlayerController::StaticClass()->GetName(), *GetName());
	}

	if (!ensure(AimWidgetClass))
	{
		UE_LOG(LogG2I, Error, TEXT("Aim Widget Class doesn't determine in %s"), *GetName());
		return;
	}
	
	AimWidget = CreateWidget<UG2IAimingWidget>(World, AimWidgetClass);

	if (!ensure(AimWidget))
	{
		UE_LOG(LogG2I, Error, TEXT("Aim Widget didn't created in %s"), *GetName());
	}
}

void UG2IAimingComponent::BindDelegates()
{
	AActor *Owner = GetOwner();
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("Owner doesn't exist in %s"), *GetName());
		return;
	}

	if (IG2ICharacterInterface *CharacterOwner = Cast<IG2ICharacterInterface>(Owner))
	{
		CharacterOwner->GetUnPossessedDelegate().AddDynamic(this, &ThisClass::StopAimingAction_Implementation);
	}
	else
	{
		UE_LOG(LogG2I, Warning, TEXT("Owner %s of %s doesn't implement %s interface"),
			*Owner->GetActorNameOrLabel(), *GetName(), *UG2ICharacterInterface::StaticClass()->GetName());
	}
	
	if (UG2ICameraControllerComponent *CameraControllerComponent = Owner->FindComponentByClass<UG2ICameraControllerComponent>())
	{
		CameraControllerComponent->OnSetCameraTypeDelegate.AddDynamic(this, &ThisClass::SetAbilityAiming);
	}

	if (UG2ISteamShotComponent *SteamShotComponent = Owner->FindComponentByClass<UG2ISteamShotComponent>())
	{
		SteamShotComponent->OnComponentActivateDelegate.AddDynamic(this, &ThisClass::SetAimDistance);
		SteamShotComponent->OnAimTypeAfterHitChangedDelegate.AddDynamic(this, &ThisClass::SetPendingAimType);
	}
}

void UG2IAimingComponent::StartAimingAction_Implementation()
{
	bWantsAiming = true;
	if (bCanAiming)
	{
		OnStartAimingDelegate.Broadcast();
		bIsAiming = true;

		if (!ensure(AimWidget))
		{
			UE_LOG(LogG2I, Error, TEXT("Aim Widget didn't created in %s"), *GetName());
			return;
		}
		AimWidget->AddToViewport();
	}
}

void UG2IAimingComponent::StopAimingAction_Implementation()
{
	bWantsAiming = false;
	if (bIsAiming)
	{
		OnFinishAimingDelegate.Broadcast();
		bIsAiming = false;

		if (!ensure(AimWidget))
		{
			UE_LOG(LogG2I, Error, TEXT("Aim Widget didn't created in %s"), *GetName());
			return;
		}
		AimWidget->RemoveFromParent();
	}
}

bool UG2IAimingComponent::IsAiming_Implementation()
{
	return bIsAiming;
}

FG2IHitInfo UG2IAimingComponent::GetAimLineHitInfo_Implementation()
{
	return AimLineHitInfo;
}

UActorComponent *UG2IAimingComponent::GetCurrentComponentUsingAim_Implementation()
{
	if (!ComponentsUsingAim.IsEmpty() &&
		ComponentsUsingAim[ComponentUsingAimIndex % ComponentsUsingAim.Num()]->
		Implements<UG2IMechanicUsingAimInterface>())
	{
		return ComponentsUsingAim[ComponentUsingAimIndex % ComponentsUsingAim.Num()];
	}
	
	return nullptr;
}

void UG2IAimingComponent::SetAbilityAiming(EG2ICameraTypeEnum CurrentCameraType)
{
	if (CurrentCameraType == EG2ICameraTypeEnum::ThirdPersonCamera)
	{
		bCanAiming = true;
		if (bWantsAiming)
		{
			StartAimingAction_Implementation();
		}
	}
	else
	{
		bCanAiming = false;
		if (bIsAiming)
		{
			StopAimingAction_Implementation();
			bWantsAiming = true;
		}
	}
}

void UG2IAimingComponent::SetAimDistance(const float NewAimDistance)
{
	CurrentAimDistance = NewAimDistance;
}

void UG2IAimingComponent::SetPendingAimType(EG2IAimType NewAimType)
{
	bAimViewIsPending = true;
	
	if (!ensure(AimWidget))
	{
		UE_LOG(LogG2I, Error, TEXT("Aim Widget didn't created in %s"), *GetName());
		return;
	}
	AimWidget->SetAimTexture(NewAimType);
}

void UG2IAimingComponent::ActivateCurrentComponentUsingAim()
{
	if (!ComponentsUsingAim.IsEmpty() &&
	ComponentsUsingAim[ComponentUsingAimIndex % ComponentsUsingAim.Num()]->
	Implements<UG2IMechanicUsingAimInterface>())
	{
		IG2IMechanicUsingAimInterface::Execute_ActivateForAim(
			ComponentsUsingAim[ComponentUsingAimIndex % ComponentsUsingAim.Num()]);
	}
}

void UG2IAimingComponent::SetAimType(const AActor* TargetActor)
{
	if (!ComponentsUsingAim.IsEmpty() &&
	ComponentsUsingAim[ComponentUsingAimIndex % ComponentsUsingAim.Num()]->
	Implements<UG2IMechanicUsingAimInterface>())
	{
		const EG2IAimType NewAimType = IG2IMechanicUsingAimInterface::Execute_GetAimTypeByActor(
			ComponentsUsingAim[ComponentUsingAimIndex % ComponentsUsingAim.Num()], TargetActor);
		if (CurrentAimType != NewAimType)
		{
			if (!bAimViewIsPending)
			{
				if (!ensure(AimWidget))
				{
					UE_LOG(LogG2I, Error, TEXT("Aim Widget didn't created in %s"), *GetName());
				}
				else
				{
					AimWidget->SetAimTexture(NewAimType);
				}
			}
			CurrentAimType = NewAimType;
		}
	}
}

void UG2IAimingComponent::DetectAimLineHitInfo()
{
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Player Controller isn't target %s in %s"),
			*AG2IPlayerController::StaticClass()->GetName(), *GetName());
		return;
	}
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}

	FVector CurrentViewLocation;
	FRotator CurrentViewRotation;
	PlayerController->GetPlayerViewPoint(CurrentViewLocation, CurrentViewRotation);
	
	const FVector CurrentViewForwardVector = CurrentViewRotation.Vector();
	const FVector AimTargetLocation = CurrentViewLocation + CurrentViewForwardVector * CurrentAimDistance;
	
	AimLineHitInfo.HitSuccess = World->LineTraceSingleByChannel(
		AimLineHitInfo.HitResult, CurrentViewLocation, AimTargetLocation,ECC_Visibility);

	if (!AimLineHitInfo.HitSuccess)
	{
		AimLineHitInfo.HitResult.Location = AimTargetLocation;
	}
	
	if (AimTargetActor != AimLineHitInfo.HitResult.GetActor())
	{
		AimTargetActor = AimLineHitInfo.HitResult.GetActor();
		SetAimType(AimTargetActor);
	}
}




#include "G2IAimingComponent.h"
#include "G2I.h"
#include "G2IMechanicUsingAimInterface.h"
#include "G2IPlayerController.h"
#include "G2ISteamShotComponent.h"
#include "G2ICameraControllerComponent.h"
#include "G2ICameraStateEnums.h"
#include "G2ICharacterInterface.h"
#include "G2IGameInstance.h"
#include "G2IOutlineComponent.h"
#include "G2ITraceableObectInterface.h"
#include "G2IUIManager.h"
#include "G2IWidgetNames.h"

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
			if (!ensure(UIManager))
			{
				UE_LOG(LogG2I, Warning, TEXT("%s isn't defined in %s"),
					*UG2IUIManager::StaticClass()->GetName(), *GetName());
			}
			UIManager->ChangeAimingType(CurrentAimType);
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

	const UG2IGameInstance *GameInstance = Cast<UG2IGameInstance>(World->GetGameInstance());
	if (ensure(GameInstance))
	{
		UIManager = GameInstance->GetSubsystem<UG2IUIManager>();
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("Game Instance doesn't exist in %s"), *GetName());
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

		if (!ensure(UIManager))
		{
			UE_LOG(LogG2I, Warning, TEXT("%s isn't defined in %s"),
				*UG2IUIManager::StaticClass()->GetName(), *GetName());
		}
		UIManager->OpenWidget(EG2IWidgetNames::Aim);
	}
	OutlineController(AimTargetActor, true);
}

void UG2IAimingComponent::StopAimingAction_Implementation()
{
	bWantsAiming = false;
	if (bIsAiming)
	{
		OnFinishAimingDelegate.Broadcast();
		bIsAiming = false;

		if (!ensure(UIManager))
		{
			UE_LOG(LogG2I, Warning, TEXT("%s isn't defined in %s"),
				*UG2IUIManager::StaticClass()->GetName(), *GetName());
		}
		UIManager->CloseWidget(EG2IWidgetNames::Aim);
	}
	OutlineController(AimTargetActor, false);
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

void UG2IAimingComponent::SetAbilityAiming(EG2ICameraTypeEnum CurrentCameraType, EG2ICameraBlendState CurrentBlendState)
{
	if (CurrentCameraType == EG2ICameraTypeEnum::ThirdPersonCamera && CurrentBlendState == EG2ICameraBlendState::Finish)
	{
		bCanAiming = true;
		if (bWantsAiming)
		{
			StartAimingAction_Implementation();
		}
		return;
	}

	if (CurrentCameraType == EG2ICameraTypeEnum::FixedCamera && CurrentBlendState == EG2ICameraBlendState::Start)
	{
		bCanAiming = false;
		if (bIsAiming)
		{
			StopAimingAction_Implementation();
			bWantsAiming = true;
		}
		return;
	}
}

void UG2IAimingComponent::SetAimDistance(const float NewAimDistance)
{
	CurrentAimDistance = NewAimDistance;
}

void UG2IAimingComponent::SetPendingAimType(EG2IAimType NewAimType)
{
	bAimViewIsPending = true;

	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s isn't defined in %s"),
			*UG2IUIManager::StaticClass()->GetName(), *GetName());
	}
	UIManager->ChangeAimingType(NewAimType);
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
				if (!ensure(UIManager))
				{
					UE_LOG(LogG2I, Warning, TEXT("%s isn't defined in %s"),
						*UG2IUIManager::StaticClass()->GetName(), *GetName());
				}
				UIManager->ChangeAimingType(NewAimType);
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

	auto PreviousAimTargetActor = AimTargetActor;
	
	if (AimTargetActor != AimLineHitInfo.HitResult.GetActor())
	{
		AimTargetActor = AimLineHitInfo.HitResult.GetActor();
		SetAimType(AimTargetActor);
		OutlineController(PreviousAimTargetActor, false);
		OutlineController(AimTargetActor, true);
	}
}

void UG2IAimingComponent::OutlineController(const AActor* ActorToChangeOutline, bool bOutlineMode)
{
	TArray<UStaticMeshComponent*> OutlineMeshes;
	if (ActorToChangeOutline && ActorToChangeOutline->Implements<UG2ITraceableObectInterface>())
	{
		ActorToChangeOutline->GetComponents<UStaticMeshComponent>(OutlineMeshes);
	}

	for (auto OutlineMesh : OutlineMeshes)
	{
		if (!ensure(OutlineMesh))
		{
			UE_LOG(LogG2I, Error, TEXT("OutlineMesh in %s is null"), *ActorToChangeOutline->GetName());
			return;
		}
		
		OutlineMesh->bDisallowNanite = bOutlineMode;
		if (bOutlineMode)
		{
			OutlineMesh->SetOverlayMaterial(ShootableObjOutlineMaterialInstance);
		}
		else
		{
			OutlineMesh->SetOverlayMaterial(nullptr);
		}
	}
	
	UG2IOutlineComponent* OutlineComp = nullptr;
	
	if (ActorToChangeOutline)
	{
		OutlineComp = ActorToChangeOutline->FindComponentByClass<UG2IOutlineComponent>();
	}

	if (OutlineComp)
	{
		OutlineComp->OutlineController(bOutlineMode);
	}
}


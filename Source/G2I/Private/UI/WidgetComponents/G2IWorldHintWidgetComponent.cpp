#include "G2IWorldHintWidgetComponent.h"
#include "G2I.h"
#include "G2ICharacterCollisionComponent.h"
#include "G2IGameInstance.h"
#include "G2IInteractiveObjectInterface.h"
#include "G2IPlayerController.h"
#include "G2IUIManager.h"
#include "G2IUserWidget.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"

UG2IWorldHintWidgetComponent::UG2IWorldHintWidgetComponent()
{
	DefaultDrawSize = GetDrawSize();
	bEnableScaleFromDistance = true;
	VisibilityZone = CreateDefaultSubobject<USphereComponent>(TEXT("Visibility zone"));
	if (!ensure(VisibilityZone))
	{
		UE_LOG(LogG2I, Error, TEXT("There is not visibility zone for widget component %s"), *GetName());
		return;
	}
	VisibilityZone->SetSphereRadius(1000.f);
}

void UG2IWorldHintWidgetComponent::OnRegister()
{
	Super::OnRegister();

	PreInitializationVisibleZone();
}

void UG2IWorldHintWidgetComponent::PreInitializationVisibleZone()
{
	if (!ensure(VisibilityZone))
	{
		UE_LOG(LogG2I, Error, TEXT("There is not visibility zone for widget component %s"), *GetName());
		return;
	}
	VisibilityZone->SetupAttachment(this);
}

void UG2IWorldHintWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	SetupDefaults();
	BindDelegates();

	ReactWidgetOnOverlappingActors();
}

void UG2IWorldHintWidgetComponent::SetupDefaults()
{
	InitializationUIManager();
	InitializationPlayerController();
	InitializationVisibilityZone();

	SetWidgetSpace(EWidgetSpace::Screen);
	SetWidgetByName(CurrentWidgetName);
}

void UG2IWorldHintWidgetComponent::InitializationUIManager()
{
	const UWorld *World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}
	const UG2IGameInstance *GameInstance = Cast<UG2IGameInstance>(World->GetGameInstance());
	if (!ensure(GameInstance))
	{
		UE_LOG(LogG2I, Error, TEXT("Game Instance doesn't exist in %s"), *GetName());
		return;
	}
	UIManager = GameInstance->GetSubsystem<UG2IUIManager>();
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s isn't defined in %s"),
			*UG2IUIManager::StaticClass()->GetName(), *GetName());
	}
}

void UG2IWorldHintWidgetComponent::InitializationPlayerController()
{
	Owner = GetOwner();
	if (!ensure(Owner))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find Owner"), *GetName());
		return;
	}
	const UWorld *World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}
	APlayerController *LocalPlayerController = World->GetFirstPlayerController();
	if (!ensure(LocalPlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Local player controller doesn't exist in %s"), *GetName());
		return;
	}

	PlayerController = Cast<AG2IPlayerController>(LocalPlayerController);
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Player Controller doesn't exist in %s"), *GetName());
		return;
	}
}

void UG2IWorldHintWidgetComponent::InitializationVisibilityZone()
{
	if (!ensure(VisibilityZone))
	{
		UE_LOG(LogG2I, Error, TEXT("There is not visibility zone for widget component %s"), *GetName());
		return;
	}
	SetCollisionResponseToAllChannels(ECR_Ignore);
	VisibilityZone->SetCollisionResponseToAllChannels(ECR_Ignore);
	VisibilityZone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	VisibilityZone->SetCollisionResponseToChannel(UG2ICharacterCollisionComponent::GetPassingThroughCollisionChannel(), ECR_Overlap);
}

void UG2IWorldHintWidgetComponent::BindDelegates()
{
	if (!ensure(VisibilityZone))
	{
		UE_LOG(LogG2I, Error, TEXT("There is not visibility zone for widget component %s"), *GetName());
	}
	else
	{
		VisibilityZone->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnVisibilityZoneBeginOverlap);
		VisibilityZone->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnVisibilityZoneEndOverlap);
	}
	
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Player Controller doesn't exist in %s"), *GetName());
		return;
	}
	PlayerController->OnPossessPawnDelegate.AddDynamic(this, &ThisClass::SetDefaultsByPlayerPawn);
}

void UG2IWorldHintWidgetComponent::SetDefaultsByPlayerPawn(APawn* Pawn)
{
	if (Pawn)
	{
		PlayerPawn = Pawn;
		PlayerPawnClass = Pawn->GetClass();
		ReactWidgetOnOverlappingActors();
	}
}

void UG2IWorldHintWidgetComponent::SetWidgetByName(const EG2IWidgetNames WidgetName)
{
	if (UG2IUserWidget *NewWidget = FindOrAddWidgetByName(WidgetName))
	{
		CurrentWidgetName = WidgetName;
		SetWidget(NewWidget);
	}
}

UG2IUserWidget *UG2IWorldHintWidgetComponent::FindOrAddWidgetByName(const EG2IWidgetNames WidgetName)
{
	if (TObjectPtr<UG2IUserWidget> *NewWidgetPtr = Widgets.Find(WidgetName))
	{
		return *NewWidgetPtr;
	}
	
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s isn't defined in %s"),
			*UG2IUIManager::StaticClass()->GetName(), *GetName());
		return nullptr;
	}
	UG2IUserWidget *NewWidget = UIManager->CreateWidgetByName(WidgetName);
	Widgets.Add(WidgetName, NewWidget);
	
	return NewWidget;
}

void UG2IWorldHintWidgetComponent::ReactWidgetOnOverlappingActors()
{
	bIsInVisibleZone = false;
	TArray<AActor*> OverlappingActors;
	if (VisibilityZone->IsOverlappingActor(PlayerPawn))
	{
		if (SetVisibleForActor(PlayerPawn))
		{
			return;
		}
	}
	CloseWidget();
}

void UG2IWorldHintWidgetComponent::OnVisibilityZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	SetVisibleForActor(OtherActor);
}

void UG2IWorldHintWidgetComponent::OnVisibilityZoneEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == PlayerPawn)
	{
		CloseWidget();
	}
}

void UG2IWorldHintWidgetComponent::SetIsLocked_Implementation(const bool bIsNewLocked)
{
	if (bIsLocked == bIsNewLocked)
	{
		return;
	}
	
	bIsLocked = bIsNewLocked;
	
	if (!VisibilityZone->IsOverlappingActor(PlayerPawn))
	{
		return;
	}
	if (bIsLocked)
	{
		CloseWidget();
	}
	else
	{
		OpenWidget();
	}
}

bool UG2IWorldHintWidgetComponent::IsLocked_Implementation()
{
	return bIsLocked;
}

bool UG2IWorldHintWidgetComponent::IsInVisibleZone() const
{
	return bIsInVisibleZone;
}

FVector2D UG2IWorldHintWidgetComponent::GetDefaultDrawSize() const
{
	return DefaultDrawSize;
}

bool UG2IWorldHintWidgetComponent::IsEnableScaleFromDistance() const
{
	return bEnableScaleFromDistance;
}

void UG2IWorldHintWidgetComponent::SetEnableScaleFromDistance(const bool bInEnableScaleFromDistance)
{
	bEnableScaleFromDistance = bInEnableScaleFromDistance;
}

bool UG2IWorldHintWidgetComponent::SetVisibleForActor(AActor* Actor)
{
	if (Actor != PlayerPawn)
	{
		return false;
	}
	if (Owner->Implements<UG2IInteractiveObjectInterface>() &&
		!IG2IInteractiveObjectInterface::Execute_CanInteract(Owner, Cast<ACharacter>(PlayerPawn)))
	{
		return false;
	}
	OpenWidget();
	return true;
}

void UG2IWorldHintWidgetComponent::OpenWidget()
{
	if (bIsLocked)
	{
		return;
	}
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s isn't defined in %s"),
			*UG2IUIManager::StaticClass()->GetName(), *GetName());
		return;
	}
	bIsInVisibleZone = true;
	UIManager->OpenWorldWidget(this);
}

void UG2IWorldHintWidgetComponent::CloseWidget()
{
	bIsInVisibleZone = false;
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s isn't defined in %s"),
			*UG2IUIManager::StaticClass()->GetName(), *GetName());
		return;
	}

	UIManager->CloseWorldWidget(this);
}
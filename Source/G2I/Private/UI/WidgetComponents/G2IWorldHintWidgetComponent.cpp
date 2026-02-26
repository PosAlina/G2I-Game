#include "G2IWorldHintWidgetComponent.h"
#include "G2I.h"
#include "G2ICharacterCollisionComponent.h"
#include "G2IGameInstance.h"
#include "G2IPlayerController.h"
#include "G2IUIManager.h"
#include "G2IUserWidget.h"
#include "Components/SphereComponent.h"

UG2IWorldHintWidgetComponent::UG2IWorldHintWidgetComponent()
{
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

	SetWidgetSize(WidgetSize);
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
		UE_LOG(LogG2I, Warning, TEXT("%s isn't defined in %s"),
			*UG2IUIManager::StaticClass()->GetName(), *GetName());
	}
}

void UG2IWorldHintWidgetComponent::InitializationPlayerController()
{
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
	PlayerController->OnPossessPawnDelegate.AddDynamic(this, &ThisClass::SetPlayerPawn);
}

void UG2IWorldHintWidgetComponent::SetPlayerPawn(APawn* Pawn)
{
	if (Pawn)
	{
		PlayerPawn = Pawn;
		ReactWidgetOnOverlappingActors();
	}
}

void UG2IWorldHintWidgetComponent::SetWidgetSize(const FVector2D InWidgetSize)
{
	WidgetSize = InWidgetSize;
	SetDrawSize(WidgetSize);
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
		UE_LOG(LogG2I, Warning, TEXT("%s isn't defined in %s"),
			*UG2IUIManager::StaticClass()->GetName(), *GetName());
		return nullptr;
	}
	UG2IUserWidget *NewWidget = UIManager->CreateWidgetByName(WidgetName);
	Widgets.Add(WidgetName, NewWidget);
	
	return NewWidget;
}

void UG2IWorldHintWidgetComponent::ReactWidgetOnOverlappingActors()
{
	TArray<AActor*> OverlappingActors;
	VisibilityZone->GetOverlappingActors(OverlappingActors);
	for (AActor *OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor == PlayerPawn)
		{
			OpenWidget();
			return;
		}
	}
	CloseWidget();
}

void UG2IWorldHintWidgetComponent::OnVisibilityZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor == PlayerPawn)
	{
		OpenWidget();
	}
}

void UG2IWorldHintWidgetComponent::OnVisibilityZoneEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == PlayerPawn)
	{
		CloseWidget();
	}
}

void UG2IWorldHintWidgetComponent::OpenWidget()
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s isn't defined in %s"),
			*UG2IUIManager::StaticClass()->GetName(), *GetName());
		return;
	}

	UIManager->OpenWorldWidget(this);
}

void UG2IWorldHintWidgetComponent::CloseWidget()
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s isn't defined in %s"),
			*UG2IUIManager::StaticClass()->GetName(), *GetName());
		return;
	}

	UIManager->CloseWorldWidget(this);
}
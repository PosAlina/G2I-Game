#include "G2IUIDisplayManager.h"
#include "G2I.h"
#include "G2IGameInstance.h"
#include "G2IPlayerCameraManager.h"
#include "G2IPlayerController.h"
#include "G2IStringTablesCatalog.h"
#include "G2IWidgetsCatalog.h"
#include "G2IWorldHintWidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Internationalization/StringTable.h"

void UG2IUIDisplayManager::Initialize()
{
	SetupDefaults();
	InitializeGameInstanceDefaults();
	BindDelegates();
}

void UG2IUIDisplayManager::RegisterWorldWidgetComponent(UG2IWorldHintWidgetComponent& WidgetComponent)
{
	const uint32 WidgetComponentID = GetWorldWidgetComponentID(WidgetComponent);
	AllWidgetComponents.Add(WidgetComponentID, &WidgetComponent);
}

uint32 UG2IUIDisplayManager::GetWorldWidgetComponentID(UG2IWorldHintWidgetComponent& WidgetComponent)
{
	return GetTypeHash(&WidgetComponent);
}

void UG2IUIDisplayManager::RegisterWidget(const EG2IWidgetNames WidgetName,
	const FG2IWidgetClassesInfo& WidgetClassInfo)
{
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}
	
	if (!AllWidgets.Contains(WidgetName) || !AllWidgets[WidgetName].Widget)
	{
		if (!ensure(WidgetClassInfo.Class))
		{
			return;
		}
		UG2IUserWidget *Widget = CreateWidget<UG2IUserWidget>(World, WidgetClassInfo.Class);
		if (!ensure(Widget))
		{
			return;
		}
		AllWidgets.Add(WidgetName, {Widget, WidgetClassInfo.Type});
	}
}

void UG2IUIDisplayManager::SetupDefaults()
{
	World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}
	PlayerController = Cast<AG2IPlayerController>(World->GetFirstPlayerController());
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("PlayerController doesn't exist in %s"), *GetName());
		return;
	}
}

void UG2IUIDisplayManager::InitializeGameInstanceDefaults()
{
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
		return;
	}
	UG2IGameInstance *GameInstance = Cast<UG2IGameInstance>(World->GetGameInstance());
	if (!ensure(GameInstance))
	{
		UE_LOG(LogG2I, Error, TEXT("Game Instance isn't %s in %s"),
			*UG2IGameInstance::StaticClass()->GetName(), *GetName());
		return;
	}
	UG2IWidgetsCatalog *WidgetsCatalog = GameInstance->GetWidgetsCatalog();
	if (!ensure(WidgetsCatalog))
	{
		UE_LOG(LogG2I, Error, TEXT("Couldn't get %s from GameInstance in %s"),
			*UG2IWidgetsCatalog::StaticClass()->GetName(), *GetName());
	}
	else
	{
		for (const auto WidgetInfo : WidgetsCatalog->WidgetClasses)
		{
			RegisterWidget(WidgetInfo.Key, WidgetInfo.Value);
		}
	}

	const UG2IStringTablesCatalog *StringTablesCatalog = GameInstance->GetStringTablesCatalog();
	if (!ensure(StringTablesCatalog))
	{
		UE_LOG(LogG2I, Error, TEXT("Couldn't get %s from GameInstance in %s"),
			*UG2IStringTablesCatalog::StaticClass()->GetName(), *GetName());
	}
	else
	{
		for (auto [TableType, Table] : StringTablesCatalog->StringTables)
		{
			if (!ensure(Table))
			{
				UE_LOG(LogG2I, Warning, TEXT("Table doesn't exist in string tables %s"), *GetName());
				continue;
			}
			StringTablesNames.Add(TableType, Table->GetStringTableId());
		}
	}
}

void UG2IUIDisplayManager::BindDelegates()
{
	PlayerController->OnPossessPawnDelegate.AddDynamic(this, &ThisClass::UpdateBindingDelegatesForChangedPawn);
	const TObjectPtr<AG2IPlayerCameraManager> CameraManager =
		Cast<AG2IPlayerCameraManager>(PlayerController->PlayerCameraManager);
	if (!CameraManager)
	{
		UE_LOG(LogG2I, Error, TEXT("Camera Manager %s doesn't exist in %s"),
			*AG2IPlayerCameraManager::StaticClass()->GetName(), *GetName());
		return;
	}
	CameraManager->OnChangeCameraLocationDelegate.AddDynamic(this,
		&ThisClass::ReactActiveWidgetComponentsToNewCameraLocation);
}

void UG2IUIDisplayManager::UpdateBindingDelegatesForChangedPawn(APawn* Pawn)
{
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Player controller doesn't exist in %s"), *GetName());
		return;
	}
	QueryParamsForWorldWidgetsActivate.ClearIgnoredSourceObjects();
	QueryParamsForWorldWidgetsActivate.AddIgnoredActor(Pawn);
}

void UG2IUIDisplayManager::ReactActiveWidgetComponentsToNewCameraLocation(const FVector& NewCameraLocation)
{
	for (const uint32 WidgetHintID : ActiveWidgetComponentsID)
	{
		if (const TObjectPtr<UG2IWorldHintWidgetComponent>* WidgetComponentPtr = AllWidgetComponents.Find(WidgetHintID))
		{
			UG2IWorldHintWidgetComponent *WidgetComponent = *WidgetComponentPtr;
			FVector WidgetLocation = WidgetComponent->GetComponentLocation();
			
			FHitResult HitResult;
			const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult,
				NewCameraLocation, WidgetLocation, ECC_Visibility,
				QueryParamsForWorldWidgetsActivate);
			WidgetComponent->SetVisibility(!bHit);
		}
	}
}

UG2IUserWidget* UG2IUIDisplayManager::GetWidget(const EG2IWidgetNames WidgetName)
{
	if (const auto WidgetInfo = AllWidgets.Find(WidgetName))
	{
		return WidgetInfo->Widget;
	}
	return nullptr;
}

UG2IUserWidget *UG2IUIDisplayManager::CreateNewWidget(const EG2IWidgetNames WidgetName)
{
	if (const UG2IUserWidget *BaseWidget = GetWidget(WidgetName))
	{
		if (!ensure(World))
		{
			UE_LOG(LogG2I, Error, TEXT("World doesn't exist in %s"), *GetName());
			return nullptr;
		}
		return CreateWidget<UG2IUserWidget>(World, BaseWidget->GetClass());
	}
	return nullptr;
}

void UG2IUIDisplayManager::OpenWidget(const EG2IWidgetNames WidgetName)
{
	if (const auto WidgetInfo = AllWidgets.Find(WidgetName))
	{
		if (UG2IUserWidget *Widget = WidgetInfo->Widget)
		{
			Widget->AddToViewport();
			
			TSet<EG2IWidgetNames> ActiveWidgetsNamesByType = AllActiveWidgetsNames.FindOrAdd(WidgetInfo->Type);
			ActiveWidgetsNamesByType.Add(WidgetName);

			if (WidgetInfo->Type == EG2IWidgetTypes::UI)
			{
				if (!ensure(PlayerController))
				{
					UE_LOG(LogG2I, Error, TEXT("Player controller doesn't exist in %s"), *GetName());
					return;
				}
				
				FInputModeUIOnly InputMode;
				InputMode.SetWidgetToFocus(Widget->TakeWidget());
				PlayerController->SetInputMode(InputMode);
				PlayerController->bShowMouseCursor = true;
			}
		}
	}
}

void UG2IUIDisplayManager::CloseWidget(const EG2IWidgetNames WidgetName)
{
	if (const auto WidgetInfo = AllWidgets.Find(WidgetName))
	{
		if (UG2IUserWidget *Widget = WidgetInfo->Widget)
		{
			Widget->RemoveFromParent();

			if (TSet<EG2IWidgetNames> *ActiveWidgetsNamesByType = AllActiveWidgetsNames.Find(WidgetInfo->Type))
			{
				ActiveWidgetsNamesByType->Remove(WidgetName);
			}
		}
	}
}

void UG2IUIDisplayManager::CloseAllActiveWidgets()
{
	for (auto [ActiveWidgetsType, _] : AllActiveWidgetsNames)
	{
		CloseActiveWidgetsByType(ActiveWidgetsType);
	}
}

void UG2IUIDisplayManager::CloseActiveWidgetsByType(const EG2IWidgetTypes WidgetsType)
{
	if (TSet<EG2IWidgetNames> *ActiveWidgetsNamesByType = AllActiveWidgetsNames.Find(WidgetsType))
	{
		for (const EG2IWidgetNames WidgetName : *ActiveWidgetsNamesByType)
		{
			CloseWidget(WidgetName);
		}
	}
}

void UG2IUIDisplayManager::ShowWorldWidget(UG2IWorldHintWidgetComponent& WidgetComponent)
{
	const uint32 WidgetComponentID = GetWorldWidgetComponentID(WidgetComponent);
	if (!AllWidgetComponents.Contains(WidgetComponentID))
	{
		RegisterWorldWidgetComponent(WidgetComponent);
	}
	
	ActiveWidgetComponentsID.Add(WidgetComponentID);
	WidgetComponent.SetVisibility(true);
}

void UG2IUIDisplayManager::HideWorldWidget(UG2IWorldHintWidgetComponent& WidgetComponent)
{
	const uint32 WidgetComponentID = GetWorldWidgetComponentID(WidgetComponent);
	if (!AllWidgetComponents.Contains(WidgetComponentID))
	{
		RegisterWorldWidgetComponent(WidgetComponent);
	}
	
	ActiveWidgetComponentsID.Remove(WidgetComponentID);
	WidgetComponent.SetVisibility(false);
}

bool UG2IUIDisplayManager::IsVisibleWorldWidget(UG2IWorldHintWidgetComponent& WidgetComponent) const
{
	const uint32 WidgetComponentID = GetWorldWidgetComponentID(WidgetComponent);
	if (ActiveWidgetComponentsID.Contains(WidgetComponentID))
	{
		return WidgetComponent.IsVisible();
	}
	return false;
}

FText UG2IUIDisplayManager::GetText(EG2IStringTablesTypes StringTableType, const FString& KeyNewText)
{
	const FName *StringTableName = StringTablesNames.Find(StringTableType);
	if (!ensure(StringTableName))
	{
		UE_LOG(LogG2I, Warning, TEXT("Target string table doesn't exist in array %s of tables %s"),
			*UG2IStringTablesCatalog::StaticClass()->GetName(), *GetName());
		return OverrideMissingStringInStringTable;
	}
	FText NewText = FText::FromStringTable((*StringTableName), KeyNewText);
	if (IsMissingOrEmptyString(NewText))
	{
		return OverrideMissingStringInStringTable;
	}
	return NewText;
}

bool UG2IUIDisplayManager::IsMissingOrEmptyString(const FText& Text) const
{
	FString Str = Text.ToString();
	Str.RemoveSpacesInline();
	return Str.IsEmpty() || Text.EqualTo(MissingStringInStringTable);
}

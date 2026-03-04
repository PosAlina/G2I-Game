#include "G2IUIDisplayManager.h"
#include "G2I.h"
#include "G2IPlayerCameraManager.h"
#include "G2IPlayerController.h"
#include "G2IWidgetsCatalog.h"
#include "G2IWorldHintWidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UG2IUIDisplayManager::Initialize(UG2IWidgetsCatalog* WidgetsCatalog)
{
	SetupDefaults();
	for (const TTuple<EG2IWidgetNames, FG2IWidgetClassesInfo> WidgetInfo : WidgetsCatalog->WidgetClasses)
	{
		RegisterWidget(WidgetInfo.Key, WidgetInfo.Value);
	}
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

void UG2IUIDisplayManager::RegisterWidget(const EG2IWidgetNames WidgetName, FG2IWidgetClassesInfo WidgetClassInfo)
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
	PlayerController->OnPossessPawnDelegate.AddDynamic(this, &ThisClass::UpdateBindingDelegatesForChangedPawn);
	const TObjectPtr<AG2IPlayerCameraManager> CameraManager = Cast<AG2IPlayerCameraManager>(PlayerController->PlayerCameraManager);
	if (!CameraManager)
	{
		return;
	}
	CameraManager->OnChangeCameraLocationDelegate.AddDynamic(this, &ThisClass::ReactActiveWidgetComponentsToNewCameraLocation);
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

void UG2IUIDisplayManager::ShowWidget(const EG2IWidgetNames WidgetName)
{
	if (const auto WidgetInfo = AllWidgets.Find(WidgetName))
	{
		if (UG2IUserWidget *Widget = WidgetInfo->Widget)
		{
			Widget->AddToViewport();

			if (WidgetInfo->Type == EG2IUIType::UI)
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

void UG2IUIDisplayManager::HideWidget(const EG2IWidgetNames WidgetName)
{
	if (const auto WidgetInfo = AllWidgets.Find(WidgetName))
	{
		if (UG2IUserWidget *Widget = WidgetInfo->Widget)
		{
			Widget->RemoveFromParent();
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

void UG2IUIDisplayManager::SetImage(const EG2IWidgetNames WidgetName, const FName ElementName, UTexture2D* Texture)
{
	if (!ensure(Texture))
	{
		UE_LOG(LogG2I, Log, TEXT("There was an attempt to change the image to a nullptr Texture in %s"),
			*GetName());
		return;
	}
	
	if (UImage *Image = GetUIElement<UImage>(WidgetName, ElementName))
	{
		Image->SetBrushFromTexture(Texture);
	}
	else
	{
		UE_LOG(LogG2I, Warning, TEXT("%s doesn't exist in %s"), *ElementName.ToString(), *GetName());
		return;
	}
}

void UG2IUIDisplayManager::SetText(const EG2IWidgetNames WidgetName, const FName ElementName, const FString& Str)
{
	const FText Text = FText::FromString(Str);
	if (UTextBlock *TextBlock = GetUIElement<UTextBlock>(WidgetName, ElementName))
	{
		TextBlock->SetText(Text);
	}
	else
	{
		UE_LOG(LogG2I, Warning, TEXT("%s doesn't exist in %s"), *ElementName.ToString(), *GetName());
		return;
	}
}

void UG2IUIDisplayManager::SetText(UG2IUserWidget* Widget, const FName ElementName, const FString& Str)
{
	const FText Text = FText::FromString(Str);
	if (UTextBlock *TextBlock = GetUIElement<UTextBlock>(Widget, ElementName))
	{
		TextBlock->SetText(Text);
	}
	else
	{
		UE_LOG(LogG2I, Warning, TEXT("%s doesn't exist in %s"), *ElementName.ToString(), *GetName());
		return;
	}
}




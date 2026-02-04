#include "G2IUIDisplayManager.h"
#include "G2I.h"
#include "G2IWidgetsCatalog.h"
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
	PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController))
	{
		UE_LOG(LogG2I, Error, TEXT("Player controller doesn't exist in %s"), *GetName());
		return;
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

void UG2IUIDisplayManager::SetText(const EG2IWidgetNames WidgetName, const FName ElementName, const FString Str)
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



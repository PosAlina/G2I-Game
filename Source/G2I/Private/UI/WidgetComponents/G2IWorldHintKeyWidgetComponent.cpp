#include "G2IWorldHintKeyWidgetComponent.h"
#include "G2I.h"
#include "G2IUIManager.h"

UG2IWorldHintKeyWidgetComponent::UG2IWorldHintKeyWidgetComponent()
{
	CurrentWidgetName = EG2IWidgetNames::PointHint;
}

void UG2IWorldHintKeyWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s isn't defined in %s"),
			*UG2IUIManager::StaticClass()->GetName(), *GetName());
		return;
	}
	UIManager->SetKeyWidgetSize(this);
	
	FindOrAddWidgetByName(EG2IWidgetNames::KeyHint);
	FindOrAddWidgetByName(EG2IWidgetNames::PointHint);

	if (OverridenInputAction)
	{
		UIManager->SetKeyByInputAction(this, OverridenInputAction);
	}
}

void UG2IWorldHintKeyWidgetComponent::OpenKeyHint(UInputAction *KeyInput)
{
	if (!OverridenInputAction)
	{
		if (!ensure(UIManager))
		{
			UE_LOG(LogG2I, Warning, TEXT("%s isn't defined in %s"),
				*UG2IUIManager::StaticClass()->GetName(), *GetName());
			return;
		}
		UIManager->SetKeyByInputAction(this, KeyInput);
	}
	SetWidgetByName(EG2IWidgetNames::KeyHint);
}

void UG2IWorldHintKeyWidgetComponent::CloseKeyHint()
{
	SetWidgetByName(EG2IWidgetNames::PointHint);
}
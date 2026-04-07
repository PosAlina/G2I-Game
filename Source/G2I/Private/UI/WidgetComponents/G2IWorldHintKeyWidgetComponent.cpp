#include "G2IWorldHintKeyWidgetComponent.h"
#include "G2I.h"
#include "G2IPlayerController.h"
#include "G2IUIManager.h"
#include "InputAction.h"
#include "GameFramework/Character.h"

UG2IWorldHintKeyWidgetComponent::UG2IWorldHintKeyWidgetComponent()
{
	CurrentWidgetName = EG2IWidgetNames::PointHint;
}

void UG2IWorldHintKeyWidgetComponent::BeginPlay()
{
	Super::BeginPlay();
	
	FindOrAddWidgetByName(EG2IWidgetNames::KeyHint);
	FindOrAddWidgetByName(EG2IWidgetNames::PointHint);

	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s isn't defined in %s"),
			*UG2IUIManager::StaticClass()->GetName(), *GetName());
		return;
	}

	if (const TObjectPtr<UInputAction> *OverrideKeyInput = OverridenInputAction.Find(PlayerPawnClass))
	{
		UIManager->SetKeyByInputAction(this, *OverrideKeyInput, PlayerPawnClass);
	}
}

// TODO: Add crossed key hint if actor is locked
void UG2IWorldHintKeyWidgetComponent::OpenKeyHint(UInputAction *KeyInput)
{
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s isn't defined in %s"),
			*UG2IUIManager::StaticClass()->GetName(), *GetName());
		return;
	}

	if (const TObjectPtr<UInputAction> *OverrideKeyInput = OverridenInputAction.Find(PlayerPawnClass))
	{
		UIManager->SetKeyByInputAction(this, *OverrideKeyInput, PlayerPawnClass);
	}
	else
	{
		UIManager->SetKeyByInputAction(this, KeyInput, PlayerPawnClass);
	}
	
	SetWidgetByName(EG2IWidgetNames::KeyHint);
}

void UG2IWorldHintKeyWidgetComponent::CloseKeyHint()
{
	SetWidgetByName(EG2IWidgetNames::PointHint);
}
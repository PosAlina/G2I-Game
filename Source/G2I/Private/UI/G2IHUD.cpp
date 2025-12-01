#include "UI/G2IHUD.h"

#include "G2I.h"
#include "G2IPlayerController.h"
#include "Blueprint/UserWidget.h"

inline void AG2IHUD::BeginPlay()
{
	Super::BeginPlay();
	InitHud();
	AddWidgetToViewport(InputDescriptionWidget);
}

void AG2IHUD::InitHud()
{
	APlayerController* pc = Cast<AG2IPlayerController>(GetOwner());
	if (!pc)
	{
		UE_LOG(LogG2I, Error, TEXT("Player controller is null"));
		return;
	}
	InputDescriptionWidget = CreateWidget<UUserWidget>(pc, InputDescriptionWidgetClass);
}

void AG2IHUD::AddWidgetToViewport(UUserWidget* WidgetToAdd)
{
	if (WidgetToAdd)
	{
		WidgetToAdd->AddToViewport();
	}
}

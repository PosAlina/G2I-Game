#include "Gameplay/G2IActivatableRectLight.h"
#include "Components/RectLightComponent.h"
#include "G2I.h"

AG2IActivatableRectLight::AG2IActivatableRectLight()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	if (!ensure(Root))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create Root Component"), *GetActorNameOrLabel());
	}
	else
	{
		SetRootComponent(Root);
	}
	
	RectLight = CreateDefaultSubobject<URectLightComponent>(TEXT("RectLight"));
	if (!ensure(RectLight))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create RectLight"), *GetActorNameOrLabel());
	}
	else
	{
		RectLight->SetupAttachment(Root);
	}
}

void AG2IActivatableRectLight::BeginPlay()
{
	Super::BeginPlay();
	
	if (!ensure(RectLight))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: RectLight is null"), *GetActorNameOrLabel());
		return;
	}
	RectLight->SetVisibility(false);
}

void AG2IActivatableRectLight::Activate_Implementation()
{
	if (!ensure(RectLight))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: RectLight is null"), *GetActorNameOrLabel());
		return;
	}
	RectLight->SetVisibility(true);
}
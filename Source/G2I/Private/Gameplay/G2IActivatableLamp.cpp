#include "Gameplay/G2IActivatableLamp.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "G2I.h"

AG2IActivatableLamp::AG2IActivatableLamp()
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

	LampBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LampBase"));
	if (!ensure(LampBase))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create LampBase"), *GetActorNameOrLabel());
	}
	else
	{
		LampBase->SetupAttachment(Root);
	}

	Lamp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lamp"));
	if (!ensure(Lamp))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create Lamp"), *GetActorNameOrLabel());
	}
	else
	{
		Lamp->SetupAttachment(LampBase);
	}

	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	if (!ensure(PointLight))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create PointLight"), *GetActorNameOrLabel());
	}
	else
	{
		PointLight->SetupAttachment(Lamp);
	}
}

void AG2IActivatableLamp::BeginPlay()
{
	Super::BeginPlay();
	
	if (!ensure(PointLight))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: PointLight is null"), *GetActorNameOrLabel());
		return;
	}
	PointLight->SetVisibility(false);
}

void AG2IActivatableLamp::Activate_Implementation()
{
	if (!ensure(PointLight))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: PointLight is null"), *GetActorNameOrLabel());
		return;
	}
	PointLight->SetVisibility(true);

	if (!ensure(Lamp) || !ensure(ActivatedMaterial))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Lamp or ActivatedMaterial is null"), *GetActorNameOrLabel());
		return;
	}
	Lamp->SetMaterial(0, ActivatedMaterial);
}
#include "Sequence/G2ILevelSequenceActivator.h"
#include "Components/BillboardComponent.h"
#include "LevelSequencePlayer.h"
#include "G2ISoundComponent.h"
#include "G2I.h"

AG2ILevelSequenceActivator::AG2ILevelSequenceActivator()
{
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RootSceneComponent);

	SoundComp = CreateDefaultSubobject<UG2ISoundComponent>(TEXT("SoundComponent"));
	if (SoundComp)
	{
		SoundComp->SetupAttachment(RootComponent);
		SoundComp->SetupSounds.Add(ActivationSoundName, FSoundConfig());
	}

#if WITH_EDITOR
	EditorSprite = CreateDefaultSubobject<UBillboardComponent>(TEXT("EditorSprite"));
	if (EditorSprite)
	{
		EditorSprite->SetupAttachment(RootComponent);

		static ConstructorHelpers::FObjectFinder<UTexture2D> SpriteTexture(TEXT("/Engine/EditorResources/S_LevelSequence"));
		if (SpriteTexture.Succeeded())
		{
			EditorSprite->SetSprite(SpriteTexture.Object);
		}
	}
#endif
}

void AG2ILevelSequenceActivator::Activate_Implementation()
{
	if (!ensure(LevelSequence))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: LevelSequence is null."), *GetActorNameOrLabel());
		return;
	}

	UWorld* World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: World is null."), *GetActorNameOrLabel());
		return;
	}

	ALevelSequenceActor* SequenceActor;
	auto* SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		World,
		LevelSequence,
		SequenceSettings,
		SequenceActor
	);
	if (!ensure(SequencePlayer))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't create SequencePlayer for the LevelSequence."), *GetActorNameOrLabel());
		return;
	}

	SequencePlayer->Play();
}

void AG2ILevelSequenceActivator::ActivateOtherActors() const
{
	if (SoundComp)
	{
		SoundComp->PlaySound(ActivationSoundId);
	}

	for (AActor* Actor : ActorsToActivate)
	{
		if (!ensure(Actor))
		{
			continue;
		}

		if (Actor->Implements<UG2IActivationInterface>())
		{
			IG2IActivationInterface::Execute_Activate(Actor);
		}

		TArray<UActorComponent*> Components;
		Actor->GetComponents(Components);

		for (UActorComponent* Component : Components)
		{
			if (Component && Component->Implements<UG2IActivationInterface>())
			{
				IG2IActivationInterface::Execute_Activate(Component);
			}
		}
	}
}

void AG2ILevelSequenceActivator::BeginPlay()
{
	Super::BeginPlay();

	if (SoundComp && SoundComp->SetupSounds.Contains(ActivationSoundName))
	{
		ActivationSoundId = SoundComp->AddSound(SoundComp->SetupSounds[ActivationSoundName]);
		if (ActivationSoundId == -1)
		{
			UE_LOG(LogG2I, Warning, TEXT("%s: Couldn't create %s (ActivationSoundId == -1)."), *GetName(), *ActivationSoundName.ToString());
		}
	}
}

#include "Sequence/G2ILevelSequenceMultiActivator.h"
#include "Components/BillboardComponent.h"
#include "LevelSequencePlayer.h"
#include "G2ISoundComponent.h"
#include "G2I.h"
#include "InputMappingContext.h"
#include "G2IPlayerController.h"

AG2ILevelSequenceMultiActivator::AG2ILevelSequenceMultiActivator()
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

void AG2ILevelSequenceMultiActivator::Activate_Implementation()
{
	++CurrentSequence;
	if (!LevelSequences.IsValidIndex(CurrentSequence))
	{
		return;
	}

	ULevelSequence* LevelSequence = LevelSequences[CurrentSequence];
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

	
	PC = Cast<AG2IPlayerController>(GetWorld()->GetFirstPlayerController());

	if (!ensure(PC))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: PlayerController is null"), *GetActorNameOrLabel());
		return;
	}
	PC->OverrideInputMappingContext({ CutsceneIMC });

	SequencePlayer->OnFinished.AddDynamic(this, &AG2ILevelSequenceMultiActivator::OnSequenceFinished);

	SequencePlayer->Play();
}

void AG2ILevelSequenceMultiActivator::ActivateOtherActors() const
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

void AG2ILevelSequenceMultiActivator::BeginPlay()
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

void AG2ILevelSequenceMultiActivator::OnSequenceFinished()
{
	if (!ensure(PC))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: PlayerController is null"), *GetActorNameOrLabel());
		return;
	}

	PC->StopOverrideInputMappingContext();
}

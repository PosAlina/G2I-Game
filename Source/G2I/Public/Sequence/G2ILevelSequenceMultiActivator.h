#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "G2IActivationInterface.h"
#include "G2ILevelSequenceMultiActivator.generated.h"

class ULevelSequence;
class UBillboardComponent;
class UG2ISoundComponent;
class UInputMappingContext;
class AG2IPlayerController;


UCLASS()
class G2I_API AG2ILevelSequenceMultiActivator : public AActor, public IG2IActivationInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound",
		meta = (Tooltip = "Sounds that could be played in the Sequence."))
	TObjectPtr<UG2ISoundComponent> SoundComp;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> CutsceneIMC;

	UPROPERTY()
	TObjectPtr<AG2IPlayerController> PC;

private:
	UPROPERTY(EditAnywhere, meta = (Tooltip = "Sequences (a.k.a. 3D cutscenes) that will be played after this actor activates in the placed order."))
	TArray<TObjectPtr<ULevelSequence>> LevelSequences;

	UPROPERTY(EditAnywhere, meta = (Tooltip = "Actors that can be activated during the Sequence."))
	TArray<TObjectPtr<AActor>> ActorsToActivate;

	UPROPERTY(EditAnywhere, meta = (Tooltip = "Settings for all of the Level Sequences."))
	FMovieSceneSequencePlaybackSettings SequenceSettings;

	UPROPERTY()
	TObjectPtr<USceneComponent> RootSceneComponent;

#if WITH_EDITORONLY_DATA // Editor-Only Sprite
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBillboardComponent> EditorSprite;
#endif

	UPROPERTY(EditAnywhere, Category = "Sound", meta = (Tooltip = "Name of the sound that will be played during sequence."))
	FName ActivationSoundName = TEXT("ActivationSound");

	int32 ActivationSoundId = -1;

	UPROPERTY()
	TMap<FName, int32> SoundIDs;

	int32 CurrentSequence = -1;

public:
	AG2ILevelSequenceMultiActivator();

	virtual void Activate_Implementation() override;

	UFUNCTION(BlueprintCallable)
	void ActivateOtherActors() const;

	UFUNCTION(BlueprintCallable)
	void PlaySound(const FName& SoundName);

	UFUNCTION()
	void OnSequenceFinished();

protected:
	virtual void BeginPlay() override;
};

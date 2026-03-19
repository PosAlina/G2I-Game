#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "G2ISoundComponent.generated.h"

class UG2IGameSoundManager;
struct FSoundConfig;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class G2I_API UG2ISoundComponent : public USceneComponent
{
	GENERATED_BODY()

private:
	TWeakObjectPtr<UG2IGameSoundManager> SoundManager;
protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditDefaultsOnly, Category = "G2I Sounds")
	TMap<FName, FSoundConfig> SetupSounds;

	int32 AddSound(const FSoundConfig* NewSoundConfig);
	bool RemoveSound(int32 SoundId);
	void RemoveAllSounds();

	bool PlaySound(int32 SoundId);
	bool StopSound(int32 SoundId);
	void StopAllSounds();

	bool ChangeSoundVolume(int32 SoundId, float NewVolume);
	bool ChangeSoundPitch(int32 SoundId, float NewPitch);
	bool ChangeSoundLocation(int32 SoundId, FVector NewLocation);
	bool ChangeSoundAttachment(int32 SoundId,
		USceneComponent* NewAttachementComponent,
		FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules::KeepRelativeTransform);
	bool ChangeSoundAttachment(int32 SoundId,
		AActor* NewAttachementActor,
		FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules::KeepRelativeTransform);
};

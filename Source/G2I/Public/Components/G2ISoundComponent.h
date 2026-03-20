#pragma once

#include "CoreMinimal.h"
//TODO Move FSoundConfig to new .h file to resolve conflict
#include "Game/G2IGameSoundManager.h"
#include "G2ISoundComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class G2I_API UG2ISoundComponent : public USceneComponent
{
	GENERATED_BODY()

private:
	TWeakObjectPtr<UG2IGameSoundManager> SoundManager;
protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sounds")
	TMap<FName, FSoundConfig> SetupSounds;


	UFUNCTION(BlueprintCallable, Category = "Sounds")
	int32 AddSound(const FSoundConfig& NewSoundConfig);

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool RemoveSound(int32 SoundId);

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	void RemoveAllSounds();

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool PlaySound(int32 SoundId);

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool StopSound(int32 SoundId);

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	void StopAllSounds();

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool ChangeSoundVolume(int32 SoundId, float NewVolume);

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool ChangeSoundPitch(int32 SoundId, float NewPitch);

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool ChangeSoundLocation(int32 SoundId, FVector NewLocation);

	bool ChangeSoundAttachment(int32 SoundId,
		USceneComponent* NewAttachementComponent,
		EAttachmentRule AttachmentRules = EAttachmentRule::SnapToTarget);
	bool ChangeSoundAttachment(int32 SoundId,
		AActor* NewAttachementActor,
		EAttachmentRule AttachmentRules = EAttachmentRule::SnapToTarget);
};

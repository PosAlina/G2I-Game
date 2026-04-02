#pragma once

#include "CoreMinimal.h"
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sounds")
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
	bool StopSound(int32 SoundId, float FadeOutTime = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	void StopAllSounds();

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool SetSoundVolume(int32 SoundId, float NewVolume);

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool SetSoundPitch(int32 SoundId, float NewPitch);

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool SetSoundLocation(int32 SoundId, FVector NewLocation);

	bool SetSoundAttachment(int32 SoundId,
		USceneComponent* NewAttachementComponent,
		EAttachmentRule AttachmentRules = EAttachmentRule::SnapToTarget);
	bool SetSoundAttachment(int32 SoundId,
		AActor* NewAttachementActor,
		EAttachmentRule AttachmentRules = EAttachmentRule::SnapToTarget);

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool SetSoundAutoDestroy(int32 SoundId, bool bNewAutoDestroy);

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool SetSoundLooping(int32 SoundId, bool bNewIsLooping);

	UFUNCTION(BlueprintPure, Category = "Sounds")
	float GetSoundVolume(int32 SoundId);

	UFUNCTION(BlueprintPure, Category = "Sounds")
	bool GetSoundLooping(int32 SoundId);

	UFUNCTION(BlueprintPure, Category = "Sounds")
	float GetSoundPitch(int32 SoundId);

	UFUNCTION(BlueprintPure, Category = "Sounds")
	FVector GetSoundLocation(int32 SoundId);

	UFUNCTION(BlueprintPure, Category = "Sounds")
	bool GetSoundAutoDestroy(int32 SoundId);
};

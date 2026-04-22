#pragma once

#include "CoreMinimal.h"
#include "Sound/G2IGameSoundManager.h"
#include "G2ISoundComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class G2I_API UG2ISoundComponent : public USceneComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TObjectPtr<UG2IGameSoundManager> SoundManager;
protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sounds")
	TMap<FName, FSoundConfig> SetupSounds;

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	int32 AddSound(const FSoundConfig& NewSoundConfig);

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool RemoveSound(const int32 SoundId);



	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool PlaySound(const int32 SoundId);

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool StopSound(const int32 SoundId, const float FadeOutTime = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	void StopAllSounds();



	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool SetSoundVolume(const int32 SoundId, const float NewVolume);

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool SetSoundPitch(const int32 SoundId, const float NewPitch);

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool SetSoundLocation(const int32 SoundId, const FVector& NewLocation);

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool SetSoundAttachmentComponent(const int32 SoundId,
		USceneComponent* NewAttachemntComponent,
		const EAttachmentRule AttachmentRules = EAttachmentRule::SnapToTarget);

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool SetSoundAttachmentActor(const int32 SoundId,
		AActor* NewAttachemntActor,
		const EAttachmentRule AttachmentRules = EAttachmentRule::SnapToTarget);

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool SetSoundAutoDestroy(const int32 SoundId, const bool bNewAutoDestroy);




	UFUNCTION(BlueprintPure, Category = "Sounds")
	float GetSoundVolume(const int32 SoundId);

	UFUNCTION(BlueprintPure, Category = "Sounds")
	float GetSoundPitch(const int32 SoundId);

	UFUNCTION(BlueprintPure, Category = "Sounds")
	FVector GetSoundLocation(const int32 SoundId);

	UFUNCTION(BlueprintPure, Category = "Sounds")
	bool GetSoundAutoDestroy(const int32 SoundId);

	UFUNCTION(BlueprintPure, Category = "Sounds")
	bool IsSoundPlaying(const int32 SoundId);
};

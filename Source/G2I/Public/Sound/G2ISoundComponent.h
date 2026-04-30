#pragma once

#include "CoreMinimal.h"
#include "Sound/G2IGameSoundManager.h"
#include "G2ISoundComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
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
	bool RemoveSound(const int32 SoundId) const;



	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool PlaySound(const int32 SoundId) const;

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool StopSound(const int32 SoundId, const float FadeOutTime = 0.0f) const;

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	void StopAllSounds() const;



	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool SetSoundVolume(const int32 SoundId, const float NewVolume) const;

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool SetSoundPitch(const int32 SoundId, const float NewPitch) const;

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool SetSoundLocation(const int32 SoundId, const FVector& NewLocation) const;

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool SetSoundAttachmentComponent(const int32 SoundId,
		USceneComponent* NewAttachmentComponent,
		const EAttachmentRule AttachmentRules = EAttachmentRule::SnapToTarget) const;

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool SetSoundAttachmentActor(const int32 SoundId,
		AActor* NewAttachmentActor,
		const EAttachmentRule AttachmentRules = EAttachmentRule::SnapToTarget) const;

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool SetSoundAutoDestroy(const int32 SoundId, const bool bNewAutoDestroy) const;

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool SetSoundPlayingOneTime(const int32 SoundId, const bool bNewIsPlayingOneTime) const;

	UFUNCTION(BlueprintCallable, Category = "Sounds")
	bool SetSoundPlayingFromStart(const int32 SoundId, const bool bNewIsPlayingFromStart) const;



	UFUNCTION(BlueprintPure, Category = "Sounds")
	float GetSoundVolume(const int32 SoundId) const;

	UFUNCTION(BlueprintPure, Category = "Sounds")
	float GetSoundPitch(const int32 SoundId) const;

	UFUNCTION(BlueprintPure, Category = "Sounds")
	FVector GetSoundLocation(const int32 SoundId) const;

	UFUNCTION(BlueprintPure, Category = "Sounds")
	bool GetSoundAutoDestroy(const int32 SoundId) const;

	UFUNCTION(BlueprintPure, Category = "Sounds")
	bool IsSoundPlaying(const int32 SoundId) const;

	UFUNCTION(BlueprintPure, Category = "Sounds")
	bool GetSoundPlayingFromStart(const int32 SoundId) const;

	UFUNCTION(BlueprintPure, Category = "Sounds")
	bool GetSoundPlayingOneTime(const int32 SoundId) const;
};
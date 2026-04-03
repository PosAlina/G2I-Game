#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Engine/EngineTypes.h"
#include "Components/AudioComponent.h"
#include "Components/SceneComponent.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
#include "G2IGameSoundManager.generated.h"

USTRUCT(BlueprintType)
struct FSoundConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Sound")
	TObjectPtr<USoundBase> Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Location")
	FVector WorldLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Attachment")
	FComponentReference AttachToComponent;

	UPROPERTY()
	USceneComponent* ResolvedAttachComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Attachment")
	EAttachmentRule AttachmentRules = EAttachmentRule::SnapToTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Play", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float VolumeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Play", meta = (ClampMin = "0.1", ClampMax = "2.0"))
	float PitchMultiplier = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Play")
	bool bIsPlayingOneTime = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Lifetime")
	bool bAutoDestroy = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Location")
	bool bIs2D = false;
};

UENUM(BlueprintType)
enum class EG2IASoundType: uint8
{
	SpeechSound UMETA(DisplayName = "Sound for dialogs"),
	MusicSound UMETA(DisplayName = "Sound for music"),
	EffectSound UMETA(DisplayName = "Sound for effects"),
	DefaultSound UMETA(DisplayName = "Default sound")
};

UCLASS()
class G2I_API UG2IGameSoundManager : public UWorldSubsystem
{
	GENERATED_BODY()
	
private:

	UPROPERTY()
	TArray<int32> IdStack;

	int32 CurrentNumberAvailable;

	UPROPERTY()
	TMap<int32, TObjectPtr<UAudioComponent>> ActiveSounds;

	void UpdateStackSize();

	float GeneralSoundMultiplier = 1.0f;
	float EffectsSoundMultiplier = 1.0f;
	float MusicSoundMultiplier = 1.0f;
	float SpeechSoundMultiplier = 1.0f;

	UPROPERTY()
	USoundMix* MainSoundMix;

	UPROPERTY()
	TMap<EG2IASoundType, USoundClass*> SoundClasses;

protected:
	
	UAudioComponent* GetAudioById(int32 SoundId);

	void OnSoundFinished(UAudioComponent* AudioComp, int32 SoundId);
public:

	UFUNCTION(BlueprintPure, Category = "Sound Manager", meta = (WorldContext = "WorldContextObject"))
	static UG2IGameSoundManager* Get(UObject* WorldContextObject);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	int32 AddSound(const FSoundConfig& NewSoundConfig);
	bool RemoveSound(int32 SoundId);
	void RemoveAllSounds();

	bool PlaySound(int32 SoundId, float FadeInTime = 0.0f);
	bool StopSound(int32 SoundId, float FadeOutTime = 0.0f);
	void StopAllSounds();

	bool SetSoundVolume(int32 SoundId, float NewVolume);
	bool SetSoundPitch(int32 SoundId, float NewPitch);
	bool SetSoundLocation(int32 SoundId, FVector NewLocation);
	bool SetSoundAttachment(int32 SoundId,
		USceneComponent* NewAttachementComponent,
		EAttachmentRule AttachmentRules = EAttachmentRule::SnapToTarget);
	bool SetSoundAttachment(int32 SoundId,
		AActor* NewAttachementActor,
		EAttachmentRule AttachmentRules = EAttachmentRule::SnapToTarget);
	bool SetSoundAutoDestroy(int32 SoundId, bool bNewAutoDestroy);
	bool SetSoundPlayingOneTime(int32 SoundId, bool bNewIsPlayingOneTime);

	float GetSoundVolume(int32 SoundId);
	float GetSoundPitch(int32 SoundId);
	FVector GetSoundLocation(int32 SoundId);
	USceneComponent* GetSoundAttachment(int32 SoundId);
	bool GetSoundAutoDestroy(int32 SoundId);
	bool GetSoundPlayingOneTime(int32 SoundId);

	UFUNCTION(BlueprintCallable, Category = "Sound|Global")
	void InitGlobalAudio(USoundMix* _MainMix, TMap<EG2IASoundType, USoundClass*> _SoundClasses);

	UFUNCTION(BlueprintCallable, Category = "Sound|Global")
	void SetGlobalVolume(EG2IASoundType SoundType, float NewVolume);

	UFUNCTION(BlueprintPure, Category = "Sound|Global")
	float GetGlobalVolume(EG2IASoundType SoundType) const;
};

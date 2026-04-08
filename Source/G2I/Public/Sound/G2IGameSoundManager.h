#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/EngineTypes.h"
#include "G2IGameSoundManager.generated.h"

class USoundBase;
class USoundClass;
class USoundMix;
class USceneComponent;
class UAudioComponent;
class UG2IGameInstance;

USTRUCT(BlueprintType)
struct FSoundConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Sound")
	TObjectPtr<USoundBase> Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Location", meta = (EditCondition = "!bIs2D", EditConditionHides))
	FVector WorldLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Attachment", meta = (EditCondition = "!bIs2D", EditConditionHides))
	FComponentReference AttachToComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> ResolvedAttachComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Attachment", meta = (EditCondition = "!bIs2D", EditConditionHides))
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
enum class EG2ISoundType: uint8
{
	SpeechSound UMETA(DisplayName = "Sound for dialogs"),
	MusicSound UMETA(DisplayName = "Sound for music"),
	EffectSound UMETA(DisplayName = "Sound for effects"),
	DefaultSound UMETA(DisplayName = "Default sound")
};

UCLASS()
class G2I_API UG2IGameSoundManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	TObjectPtr<UG2IGameInstance> GameInstance;

	FDelegateHandle StartGameDelegateHandle;

	UPROPERTY()
	TArray<int32> IdStack;

	int32 CurrentNumberAvailable;

	UPROPERTY()
	TMap<int32, TObjectPtr<UAudioComponent>> ActiveSounds;

	void UpdateStackSize();
	UPROPERTY()
	TMap<EG2ISoundType, float> GlobalSoundMultipliers;

	UPROPERTY()
	TObjectPtr<USoundMix> MainSoundMix;

	UPROPERTY()
	TMap<EG2ISoundType, TObjectPtr<USoundClass>> SoundClasses;

protected:
	void InitializeInStartGame();
	void InitializeInStartLevel();
	void CloseLevelSound();

	TObjectPtr<UAudioComponent> GetAudioById(const int32 SoundId) const;

	void OnSoundFinished(UAudioComponent* AudioComp, const int32 SoundId);
public:

	UFUNCTION(BlueprintPure, Category = "Sound Manager", meta = (WorldContext = "WorldContextObject"))
	static UG2IGameSoundManager* Get(const UObject* WorldContextObject);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	int32 AddSound(const FSoundConfig& NewSoundConfig);
	bool RemoveSound(const int32 SoundId);
	void RemoveAllSounds();

	bool PlaySound(const int32 SoundId, const float FadeInTime = 0.0f);
	bool StopSound(const int32 SoundId, const float FadeOutTime = 0.0f);
	void StopAllSounds();

	bool SetSoundVolume(const int32 SoundId, const float NewVolume);
	bool SetSoundPitch(const int32 SoundId, const float NewPitch);
	bool SetSoundLocation(const int32 SoundId, const FVector& NewLocation);
	bool SetSoundAttachment(const int32 SoundId,
		USceneComponent* NewAttachemntComponent,
		const EAttachmentRule AttachmentRules = EAttachmentRule::SnapToTarget);
	bool SetSoundAttachment(const int32 SoundId,
		AActor* NewAttachemntActor,
		const EAttachmentRule AttachmentRules = EAttachmentRule::SnapToTarget);
	bool SetSoundAutoDestroy(const int32 SoundId, const bool bNewAutoDestroy);
	bool SetSoundPlayingOneTime(const int32 SoundId, const bool bNewIsPlayingOneTime);

	float GetSoundVolume(const int32 SoundId) const;
	float GetSoundPitch(const int32 SoundId) const;
	FVector GetSoundLocation(const int32 SoundId) const;
	TObjectPtr<USceneComponent> GetSoundAttachment(const int32 SoundId) const;
	bool GetSoundAutoDestroy(const int32 SoundId) const;
	bool GetSoundPlayingOneTime(const int32 SoundId) const;

	UFUNCTION(BlueprintCallable, Category = "Sound|Global")
	void InitGlobalAudio(USoundMix* _MainMix, const TMap<EG2ISoundType, USoundClass*>& _SoundClasses);

	UFUNCTION(BlueprintCallable, Category = "Sound|Global")
	void SetGlobalVolume(const EG2ISoundType SoundType, const float NewVolume);

	UFUNCTION(BlueprintPure, Category = "Sound|Global")
	float GetGlobalVolume(const EG2ISoundType SoundType) const;
};

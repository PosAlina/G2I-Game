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
class UG2IOptionsParameters;

USTRUCT(BlueprintType)
struct FG2IActiveSoundData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound Data")
	TObjectPtr<UAudioComponent> AudioComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound Data")
	bool bIsPlayingFromStart = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound Data")
	bool bIsPlayingOneTime = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound Data")
	bool bIsPlayed = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound Data")
	float Delay = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sound Data")
	FTimerHandle PlayTimerHandle;
};

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Play", meta = (ClampMin = "0.0", ClampMax = "10.0"))
	float VolumeMultiplier = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Play", meta = (ClampMin = "0.1", ClampMax = "2.0"))
	float PitchMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Play", meta = (ClampMin = "0.0"))
	float Delay = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Play")
	bool bIsPlayingOneTime = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Play")
	bool bIsPlayingFromStart = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Lifetime")
	bool bAutoDestroy = false;

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
	TObjectPtr<UG2IOptionsParameters> OptionsParameters;

	UPROPERTY()
	TObjectPtr<UG2IGameInstance> GameInstance;

	FDelegateHandle StartGameDelegateHandle;

	UPROPERTY()
	TArray<int32> IdStack;

	int32 CurrentNumberAvailable;

	UPROPERTY()
	TMap<int32, FG2IActiveSoundData> ActiveSounds;

	void UpdateStackSize();
	UPROPERTY()
	TMap<EG2ISoundType, float> GlobalSoundMultipliers;

	UPROPERTY()
	TObjectPtr<USoundMix> MainSoundMix;

	UPROPERTY()
	TMap<EG2ISoundType, TObjectPtr<USoundClass>> SoundClasses;

protected:
	void ExecuteDelayedPlay(const int32 SoundId, const float FadeInTime);

	void InitializeInStartGame();
	void InitializeInStartLevel();
	void CloseLevelSound();

	TObjectPtr<UAudioComponent> GetAudioById(const int32 SoundId) const;

	FG2IActiveSoundData* GetSoundDataById(const int32 SoundId);

	void OnSoundFinished(UAudioComponent* AudioComp, const int32 SoundId);
public:

	UFUNCTION(BlueprintPure, Category = "Sound Manager", meta = (WorldContext = "WorldContextObject"))
	static UG2IGameSoundManager* Get(const UObject* WorldContextObject);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Sound Manager")
	int32 AddSound(const FSoundConfig& NewSoundConfig);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager")
	bool RemoveSound(const int32 SoundId);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager")
	void RemoveAllSounds();

	UFUNCTION(BlueprintCallable, Category = "Sound Manager")
	bool PlaySound(const int32 SoundId, const float FadeInTime = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager")
	bool StopSound(const int32 SoundId, const float FadeOutTime = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager")
	void StopAllSounds();

	UFUNCTION(BlueprintCallable, Category = "Sound Manager")
	bool SetSoundVolume(const int32 SoundId, const float NewVolume);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager")
	bool SetSoundPitch(const int32 SoundId, const float NewPitch);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager")
	bool SetSoundLocation(const int32 SoundId, const FVector& NewLocation);

	bool SetSoundAttachment(const int32 SoundId, USceneComponent* NewAttachemntComponent, const EAttachmentRule AttachmentRules = EAttachmentRule::SnapToTarget);
	bool SetSoundAttachment(const int32 SoundId, AActor* NewAttachemntActor, const EAttachmentRule AttachmentRules = EAttachmentRule::SnapToTarget);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager")
	bool SetSoundAutoDestroy(const int32 SoundId, const bool bNewAutoDestroy);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager")
	bool SetSoundPlayingOneTime(const int32 SoundId, const bool bNewIsPlayingOneTime);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager")
	bool SetSoundPlayingFromStart(const int32 SoundId, const bool bNewIsPlayingFromStart);

	UFUNCTION(BlueprintCallable, Category = "Sound Manager")
	bool SetSoundDelay(const int32 SoundId, const float NewDelay);



	UFUNCTION(BlueprintPure, Category = "Sound Manager")
	float GetSoundVolume(const int32 SoundId) const;

	UFUNCTION(BlueprintPure, Category = "Sound Manager")
	float GetSoundPitch(const int32 SoundId) const;

	UFUNCTION(BlueprintPure, Category = "Sound Manager")
	FVector GetSoundLocation(const int32 SoundId) const;

	UFUNCTION(BlueprintPure, Category = "Sound Manager")
	USceneComponent* GetSoundAttachment(const int32 SoundId) const;

	UFUNCTION(BlueprintPure, Category = "Sound Manager")
	bool GetSoundAutoDestroy(const int32 SoundId) const;

	UFUNCTION(BlueprintPure, Category = "Sound Manager")
	bool GetSoundPlayingOneTime(const int32 SoundId) const;

	UFUNCTION(BlueprintCallable, Category = "Sound Manager")
	bool GetSoundPlayingFromStart(const int32 SoundId) const;

	UFUNCTION(BlueprintCallable, Category = "Sound Manager")
	float GetSoundDelay(const int32 SoundId) const;

	UFUNCTION(BlueprintPure, Category = "Sound Manager")
	bool IsSoundPlaying(const int32 SoundId) const;




	UFUNCTION(BlueprintCallable, Category = "Sound|Global")
	void InitGlobalAudio(USoundMix* _MainMix, const TMap<EG2ISoundType, USoundClass*>& _SoundClasses);




	UFUNCTION(BlueprintCallable, Category = "Sound|Global")
	void SetGlobalVolume(const EG2ISoundType SoundType, const float NewVolume);

	UFUNCTION(BlueprintPure, Category = "Sound|Global")
	float GetGlobalVolume(const EG2ISoundType SoundType) const;
};

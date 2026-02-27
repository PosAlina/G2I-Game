#pragma once



#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "G2IGameSoundManager.generated.h"


class UAudioComponent;

USTRUCT(BlueprintType)
struct FSoundConfig 
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Sound")
	TObjectPtr<USoundCue> Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Location")
	FVector WorldLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Attachment")
	TObjectPtr<USceneComponent> AttachToComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Attachment")
	FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules::SnapToTargetIncludingScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Play", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float VolumeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Play", meta = (ClampMin = "0.1", ClampMax = "2.0"))
	float PitchMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound|Lifetime")
	bool bAutoDestroy = true;
};

UCLASS()
class G2I_API UG2IGameSoundManager : public UWorldSubsystem
{
	GENERATED_BODY()
	
private:

	UPROPERTY()
	TArray<int32> IdStack;
	
	UPROPERTY()
	TMap<int32, TObjectPtr<UAudioComponent>> ActiveSounds;

protected:
	
	TObjectPtr<UAudioComponent> GetAudioById(int32 SoundId);
public:

	UFUNCTION(BlueprintPure, Category = "Sound Manager")
	static TObjectPtr<UG2IGameSoundManager> Get(UObject* WorldContextObject);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	int32 AddSound(TObjectPtr<FSoundConfig> NewSoundConfig);
	bool RemoveSound(int32 SoundId);
	void RemoveAllSounds();

	bool PlaySound(int32 SoundId);
	bool StopSound(int32 SoundId);
	void StopAllSounds();

	bool ChangeSoundVolume(int32 SoundId, float NewVolume);
	bool ChangeSoundPitch(int32 SoundId, float NewPitch);
	bool ChangeSoundLocation(int32 SoundId, FVector NewLocation);
	bool ChangeSoundAttachment(int32 SoundId,
		TObjectPtr<USceneComponent> NewAttachementComponent,
		FAttachmentTransformRules AttachmentRules);
	bool ChangeSoundAttachment(int32 SoundId,
		TObjectPtr<AActor> NewAttachementActor,
		FAttachmentTransformRules AttachmentRules);
};

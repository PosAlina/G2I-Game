#pragma once

#include "CoreMinimal.h"
#include "G2IActivationInterface.h"
#include "GameFramework/Actor.h"
#include "G2ICheckerLight.generated.h"

class UG2ILauncherComponent;
class UG2ISoundComponent;

UCLASS()
class G2I_API AG2ICheckerLight : public AActor, public IG2IActivationInterface
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UG2ILauncherComponent> LauncherComp;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UG2ISoundComponent> SoundComp;
	
	UPROPERTY(EditAnywhere, Category = "Activation")
	bool bIsLampActivated = false;
	
	UPROPERTY(EditAnywhere, Category = "Activation|Activate|Sound")
	bool bActivateSoundEnable = true;
	
	UPROPERTY(EditAnywhere, Category = "Activation|Deactivate|Sound")
	bool bDeactivateSoundEnable = true;
	
private:
	
	static inline const FName ActivateLampSoundName = FName("ActivateLamp");
	int32 ActivateSoundID = -1;
	
	static inline const FName DeactivateLampSoundName = FName("DeactivateLamp");
	int32 DeactivateSoundID = -1;
	
	bool bIsInitialized = false;
	
public:
	
	AG2ICheckerLight();
	
	UFUNCTION(BlueprintCallable, Category = "Activation")
	virtual void Activate_Implementation() override;
	
	UFUNCTION(BlueprintCallable, Category = "Activation")
	virtual void Deactivate_Implementation() override;

protected:
	
	virtual void BeginPlay() override;
	
	virtual void SetVisualActivateLight() {}
	
	virtual void SetVisualDeactivateLight() {}
	
	void AddSoundLamp(const FName& SoundName, int32& SoundID) const;
	
	void PlaySound(const int32& SoundID) const;
	
	UFUNCTION()
	void Lock(UG2ILauncherComponent* LauncherComponent, AActor* ComponentOwner, bool bIsLocked);
	
private:
	
	void BindDelegates();

};

#pragma once

#include "CoreMinimal.h"
#include "G2IDestructibleActorInterface.h"
#include "GameFramework/Actor.h"
#include "G2IDestructibleActorBase.generated.h"

class UG2ISoundComponent;
class UNiagaraSystem;

DECLARE_MULTICAST_DELEGATE(FOnDestroyed);

UCLASS()
class G2I_API AG2IDestructibleActorBase : public AActor, public IG2IDestructibleActorInterface
{
	GENERATED_BODY()
	
public:	
	virtual void DestroySelf_Implementation() override;
	AG2IDestructibleActorBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMesh> DestroyedMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> DestructibleMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UG2ISoundComponent> SoundComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UNiagaraSystem> NiagaraSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SoundName = FName("DestructSound");
	
	FOnDestroyed OnDestroyedDelegate;

protected:
	virtual void BeginPlay() override;
	
private:
	bool bIsDestroyed = false;
	int32 SoundID = -1;
};
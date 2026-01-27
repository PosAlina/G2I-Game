

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/G2ITraceableObectInterface.h"
#include "Interfaces/G2IInteractiveObjectInterface.h"
#include "G2IMovingBySteamAndHandsObject.generated.h"

UCLASS()
class G2I_API AG2IMovingBySteamAndHandsObject : public AActor, public IG2ITraceableObectInterface, public IG2IInteractiveObjectInterface
{
	GENERATED_BODY()
	
protected:	
	AG2IMovingBySteamAndHandsObject();

public:

	bool CanInteract_Implementation(const ACharacter* Interactor) override;

	void Interact_Implementation(const ACharacter* Interactor) override;

	void OnShoot_Implementation(const FHitResult& HitResult, AActor* Character) override;

	UPROPERTY(EditAnywhere, Category = "Data")
	TSet<ACharacter*> PossibleInteractors;
};

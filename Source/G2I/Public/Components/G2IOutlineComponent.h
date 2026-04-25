#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "G2IOutlineComponent.generated.h"

class AG2IPlayerController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class G2I_API UG2IOutlineComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UMaterialInstance> OutlineMaterialInstance;
	
private:
	
	UPROPERTY()
	TObjectPtr<AG2IPlayerController> PlayerController;

public:

	virtual void BeginPlay() override;

	void OutlineController(bool bOutlineMode) const;
	
protected:
	
	UFUNCTION()
	void BindDelegatesByPawn(APawn *Pawn);
	
	UFUNCTION()
	void UnBindDelegatesByPawn(APawn *Pawn);
	
	UFUNCTION()
	void StartOutline();

	UFUNCTION()
	void StopOutline();
	
private:
	
	void SetupDefaults();
	void BindDelegates();
	
};

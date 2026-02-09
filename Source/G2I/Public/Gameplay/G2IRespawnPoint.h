#pragma once

#include "G2I.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "G2IRespawnPoint.generated.h"

UCLASS()
class G2I_API AG2IRespawnPoint : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UArrowComponent> ArrowComponent;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootSceneComponent;

public:
	// Sets default values for this actor's properties
	AG2IRespawnPoint();

protected:
	virtual void BeginPlay() override;
};

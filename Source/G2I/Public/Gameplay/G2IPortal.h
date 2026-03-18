#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "G2IPortal.generated.h"

class ACharacter;
class UArrowComponent;
class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class G2I_API AG2IPortal : public AActor
{
	GENERATED_BODY()
	
public:	
	AG2IPortal();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UArrowComponent> Arrow;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> PortalMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UBoxComponent> TriggerBox;

    UFUNCTION()
    void OverlapTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Portal")
    TObjectPtr<AG2IPortal> TargetPortal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
    bool bIsActive = true;

    UFUNCTION(BlueprintCallable)
    void Teleport(ACharacter* Interactor);
};

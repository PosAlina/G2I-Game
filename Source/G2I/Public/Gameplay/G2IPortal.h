#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "G2IPortal.generated.h"

class ACharacter;
class UArrowComponent;
class UStaticMeshComponent;
class UBoxComponent;
class AG2IPlayerController;

UCLASS()
class G2I_API AG2IPortal : public AActor
{
	GENERATED_BODY()
	
public:	
	AG2IPortal();

protected:
    virtual void BeginPlay() override;

    virtual void OnConstruction(const FTransform& Transform) override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UArrowComponent> Arrow;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UArrowComponent> CameraRotationArrow;

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
    void Teleport(ACharacter* Interactor) const;

    UPROPERTY()
    TObjectPtr<AG2IPlayerController> PlayerController;
};

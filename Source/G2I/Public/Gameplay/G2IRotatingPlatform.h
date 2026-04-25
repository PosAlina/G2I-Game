#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "G2IActivationInterface.h"
#include "G2IRotatingPlatform.generated.h"

class UStaticMeshComponent;
class URotatingMovementComponent;


UCLASS()
class G2I_API AG2IRotatingPlatform : public AActor, public IG2IActivationInterface
{
	GENERATED_BODY()
	
public:	
	AG2IRotatingPlatform();

	UFUNCTION(BlueprintCallable, Category = "Rotation")
	virtual void Activate_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "Rotation")
	virtual void Deactivate_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "Rotation")
	void SetRotationSpeed(FRotator NewRotationSpeed);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> PlatformMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<URotatingMovementComponent> RotatingComponent;

public:
	UPROPERTY(EditAnywhere, Category = "Movement")
	FRotator RotationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bShouldRotate = true;

};

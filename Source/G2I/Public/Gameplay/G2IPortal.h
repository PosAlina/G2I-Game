

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "G2IPortal.generated.h"

class ACharacter;
class UArrowComponent;


UCLASS()
class G2I_API AG2IPortal : public AActor
{
	GENERATED_BODY()
	
public:	
	AG2IPortal();

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UArrowComponent> Arrow;

public:	
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Portal")
    TObjectPtr<AG2IPortal> Portal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal")
    bool bIsActive = false;

    UFUNCTION(BlueprintCallable)
    void Teleport(ACharacter* Interactor);
};

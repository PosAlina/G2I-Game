#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "G2IOutlineComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class G2I_API UG2IOutlineComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	void OutlineController(bool bOutlineMode);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UMaterialInstance> OutlineMaterialInstance;
};

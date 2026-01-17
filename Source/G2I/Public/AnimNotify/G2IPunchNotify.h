#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "G2IPunchNotify.generated.h"

UCLASS()
class G2I_API UG2IPunchNotify : public UAnimNotify
{
	GENERATED_BODY()
	
protected:	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};

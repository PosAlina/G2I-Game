#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "G2ITasksCatalog.generated.h"

enum class EG2ITasksNames : uint8;

USTRUCT(BlueprintType)
struct FG2ITasksDescriptionsInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FText Description;
};

/**
 * Tasks (now for training) with descriptions
 */
UCLASS()
class G2I_API UG2ITasksCatalog : public UDataAsset
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere)
	TMap<EG2ITasksNames, FG2ITasksDescriptionsInfo> Tasks;
	
};

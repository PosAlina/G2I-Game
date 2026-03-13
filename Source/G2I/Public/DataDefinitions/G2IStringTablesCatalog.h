#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "G2IStringTablesCatalog.generated.h"

enum class EG2IStringTablesTypes : uint8;

/**
 * List of all string tables
 */
UCLASS()
class G2I_API UG2IStringTablesCatalog : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere)
	TMap<EG2IStringTablesTypes, TObjectPtr<UStringTable>> StringTables;
	
};

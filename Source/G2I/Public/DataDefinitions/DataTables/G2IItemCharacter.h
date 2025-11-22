#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Templates/SubclassOf.h"
#include "G2IItemCharacter.generated.h"

/**
 * Row for table of playable characters.
 * Contains from enum of characters name, certain character pointer.
 */
USTRUCT(BlueprintType)
struct G2I_API FG2IItemCharacter : public FTableRowBase
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Chracter)
	TSubclassOf<AActor> CharacterClass;
};

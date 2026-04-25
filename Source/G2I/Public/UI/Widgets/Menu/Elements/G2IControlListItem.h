#pragma once

#include "CoreMinimal.h"
#include "G2IControlListItem.generated.h"

/**
 * Object for creation element of controls list
 */
UCLASS()
class UG2IControlListItem : public UObject
{
	GENERATED_BODY()

public:

	FText ActionName;
	FText KeyName;
	
};

#pragma once

#include "CoreMinimal.h"
#include "G2IUserWidget.h"
#include "G2IAimingWidget.generated.h"

UCLASS()
class G2I_API UG2IAimingWidget : public UG2IUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> AimImage;

};

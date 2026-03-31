#pragma once

#include "CoreMinimal.h"
#include "G2IUserWidget.h"
#include "G2ILevelLoadingScreen.generated.h"

class UProgressBar;

UCLASS()
class G2I_API UG2ILevelLoadingScreen : public UG2IUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> LoadingProgressBar;

public:

	void SetLoadingProgress(const float InValue) const;
};

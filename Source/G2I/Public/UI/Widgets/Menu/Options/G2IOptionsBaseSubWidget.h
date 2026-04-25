#pragma once

#include "CoreMinimal.h"
#include "G2IUpdateOptions.h"
#include "G2IUserWidget.h"
#include "G2IOptionsBaseSubWidget.generated.h"

class UG2IOptionsParameters;
/**
 * Parent of all options widgets
 */
UCLASS()
class G2I_API UG2IOptionsBaseSubWidget : public UG2IUserWidget, public IG2IUpdateOptions
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY()
	TObjectPtr<UG2IOptionsParameters> OptionsParameters;
	
public:

	virtual void ApplyOptions_Implementation() override {}
	virtual void CancelUnAppliedOptions_Implementation() override {}
	
protected:
	
	virtual void InitializeAfterManagerLoading() override;
	
};

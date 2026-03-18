#pragma once

#include "CoreMinimal.h"
#include "G2IUserWidget.h"
#include "G2IAimingWidget.generated.h"

enum class EG2IAimType : uint8;
class UImage;

UCLASS()
class G2I_API UG2IAimingWidget : public UG2IUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> AimImage;

protected:

	UPROPERTY(EditAnywhere, Category = Aiming, meta = (ToolTip = "List of images for different aim states"))
	TMap<EG2IAimType, TObjectPtr<UTexture2D>> AimingViewTextures;

public:

	void SetAimingViewType(EG2IAimType Type);
	
protected:

	virtual void InitializeAfterManagerLoading() override;
	
};

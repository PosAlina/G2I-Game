#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "G2IAimingWidget.generated.h"

enum class EG2IAimType : uint8;
class UCanvasPanel;
class UImage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAimPositionChanged, FVector2D, AimPosition);

UCLASS()
class G2I_API UG2IAimingWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	FAimPositionChanged OnAimPositionChanged;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> RootCanvasPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> AimImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "List of images for different sight states"))
	TMap<EG2IAimType, TObjectPtr<UTexture2D>> AimTextureMap;
	
public:
	
	virtual void NativeConstruct() override;

	void SetAimTexture(EG2IAimType AimType);

};

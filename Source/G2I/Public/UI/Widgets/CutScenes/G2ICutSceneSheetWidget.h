#pragma once

#include "CoreMinimal.h"
#include "G2IUserWidget.h"
#include "G2ICutSceneSheetWidget.generated.h"

class UImage;

USTRUCT(BlueprintType)
struct FG2IFrameSetOneClick
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = Frames, meta = (ToolTip = "List of frame's images for one click"))
	TSet<TObjectPtr<UImage>> FramesOneClick;
};

/**
 * Cut scene's sheet with some frames
 */
UCLASS()
class G2I_API UG2ICutSceneSheetWidget : public UG2IUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadWrite, Category = Frames, meta = (ToolTip = "List of frame's images"))
	TArray<FG2IFrameSetOneClick> FrameImages;

private:
	
	int32 CurrentFrameIndex = -1;

public:
	
	bool ShowNextFrames();
	
	void HideAllFrames();
	
};

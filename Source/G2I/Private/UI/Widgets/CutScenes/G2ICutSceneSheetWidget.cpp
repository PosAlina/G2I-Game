#include "CutScenes/G2ICutSceneSheetWidget.h"
#include "Components/Image.h"

bool UG2ICutSceneSheetWidget::ShowNextFrames()
{
	const int32 FrameSetsNum = FrameImages.Num();
	++CurrentFrameIndex;
	if (CurrentFrameIndex < FrameSetsNum && FrameImages.IsValidIndex(CurrentFrameIndex))
	{
		for (UImage *Frame : FrameImages[CurrentFrameIndex].FramesOneClick)
		{
			if (Frame)
			{
				Frame->SetVisibility(ESlateVisibility::Visible);
			}
		}
		return true;
	}
	return false;
}

void UG2ICutSceneSheetWidget::HideAllFrames()
{
	for (const auto& [FramesOneClick] : FrameImages)
	{
		for (UImage *Image : FramesOneClick)
		{
			if (!Image)
			{
				continue;
			}
			Image->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	CurrentFrameIndex = -1;
}

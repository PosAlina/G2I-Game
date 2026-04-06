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
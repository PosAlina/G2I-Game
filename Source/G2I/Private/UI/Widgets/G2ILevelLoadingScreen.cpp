#include "G2ILevelLoadingScreen.h"

#include "G2I.h"
#include "Components/ProgressBar.h"

void UG2ILevelLoadingScreen::SetLoadingProgress(const float InValue) const
{
	if (!ensure(LoadingProgressBar))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find LoadingProgressBar"), *GetName());
	}
	LoadingProgressBar->SetPercent(InValue);
}

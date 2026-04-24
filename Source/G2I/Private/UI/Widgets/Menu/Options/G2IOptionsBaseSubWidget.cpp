#include "Menu/Options/G2IOptionsBaseSubWidget.h"
#include "G2I.h"
#include "G2IGameInstance.h"
#include "G2IOptionsParameters.h"

void UG2IOptionsBaseSubWidget::InitializeAfterManagerLoading()
{
	if (!ensure(GameInstance))
	{
		UE_LOG(LogG2I, Error, TEXT("Game Instance doesn't exist in %s"), *GetName());
		return;
	}
	OptionsParameters = GameInstance->GetOptionsParameters();
	if (!ensure(OptionsParameters))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"),
			*GetName(), *UG2IOptionsParameters::StaticClass()->GetName());
		return;
	}
}

#include "Menu/G2ICreatorsWidget.h"
#include "G2I.h"
#include "G2IUIManager.h"
#include "G2IWidgetNames.h"
#include "Components/Button.h"

void UG2ICreatorsWidget::InitializeAfterManagerLoading()
{
	Super::InitializeAfterManagerLoading();

	BindDelegates();
}

void UG2ICreatorsWidget::BindDelegates()
{
	if (ensure(BackButton))
	{
		BackButton->OnClicked.AddDynamic(this, &ThisClass::OnBackButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find BackButton"), *GetName());
	}
}

void UG2ICreatorsWidget::OnBackButtonClicked()
{
	if (ensure(OnBack))
	{
		OnBack();
	}
	else
	{
		UE_LOG(LogG2I, Warning, TEXT("Back function is undefined in %s"), *GetName());
	}
	
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->CloseWidget(EG2IWidgetNames::Creators);
}

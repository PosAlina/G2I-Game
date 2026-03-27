#include "G2IConfirmationWidget.h"
#include "G2I.h"
#include "G2IUIManager.h"
#include "G2IWidgetNames.h"
#include "Components/Button.h"

void UG2IConfirmationWidget::InitializeAfterManagerLoading()
{
	Super::InitializeAfterManagerLoading();

	BindDelegates();
}

void UG2IConfirmationWidget::BindDelegates()
{
	if (ensure(ConfirmButton))
	{
		ConfirmButton->OnClicked.AddDynamic(this, &ThisClass::OnConfirmButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find ConfirmButton"), *GetName());
	}

	if (ensure(CancelButton))
	{
		CancelButton->OnClicked.AddDynamic(this, &ThisClass::OnCancelButtonClicked);
	}
	else
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find CancelButton"), *GetName());
	}
}

void UG2IConfirmationWidget::OnConfirmButtonClicked()
{
	if (ensure(OnConfirm))
	{
		OnConfirm();
	}
	else
	{
		UE_LOG(LogG2I, Warning, TEXT("Confirm function is undefined in %s"), *GetName());
	}

	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->CloseWidget(EG2IWidgetNames::Confirmation);
}

void UG2IConfirmationWidget::OnCancelButtonClicked()
{
	if (ensure(OnCancel))
	{
		OnCancel();
	}
	else
	{
		UE_LOG(LogG2I, Warning, TEXT("Cancel function is undefined in %s"), *GetName());
	}
	
	if (!ensure(UIManager))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(),
			*UG2IUIManager::StaticClass()->GetName());
		return;
	}
	UIManager->CloseWidget(EG2IWidgetNames::Confirmation);
}

#include "HUD/G2IAimingWidget.h"
#include "G2I.h"
#include "G2IAimTypeEnum.h"
#include "Components/Image.h"

void UG2IAimingWidget::SetAimingViewType(const EG2IAimType Type)
{
	if (!ensure(AimImage))
	{
		UE_LOG(LogG2I, Error, TEXT("Aiming Image isn't binded to anything in %s"), *GetName());
		return;
	}
	if (const TObjectPtr<UTexture2D> *Texture = AimingViewTextures.Find(Type))
	{
		if (!ensure(*Texture))
		{
			UE_LOG(LogG2I, Error, TEXT("Texture for aiming isn't set in %s"), *GetName());
			return;
		}
		AimImage->SetBrushFromTexture(*Texture);
	}
}

void UG2IAimingWidget::InitializeAfterManagerLoading()
{
	Super::InitializeAfterManagerLoading();

	SetAimingViewType(EG2IAimType::DefaultAim);
}
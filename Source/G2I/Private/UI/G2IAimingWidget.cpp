#include "G2IAimingWidget.h"
#include "G2I.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "G2IAimTypeEnum.h"

void UG2IAimingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetAimTexture(EG2IAimType::DefaultAim);
}

void UG2IAimingWidget::SetAimTexture(const EG2IAimType AimType)
{
	if (!AimImage)
	{
		UE_LOG(LogG2I, Error, TEXT("Aim Image doesn't exist in %s"), *GetName());
		return;
	}
	
	if (const TObjectPtr<UTexture2D> *AimTexture = AimTextureMap.Find(AimType))
	{
		AimImage->SetBrushFromTexture(*AimTexture);
	}
}

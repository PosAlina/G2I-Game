


#include "Gameplay/G2ISliderLampComponent.h"

#include "BaseGizmos/GizmoElementRenderState.h"
#include "Components/PointLightComponent.h"

UG2ISliderLampComponent::UG2ISliderLampComponent()
{
	LampMesh = CreateDefaultSubobject<UStaticMeshComponent>("LampMesh");
	
	LampMesh->SetupAttachment(this);
}

void UG2ISliderLampComponent::BeginPlay()
{
	Super::BeginPlay();
	
	DynamicMaterial = UMaterialInstanceDynamic::Create(LightMaterial, this);
	DynamicMaterial->SetVectorParameterValue("BaseColor", LampColor);
	DynamicMaterial->SetVectorParameterValue("EmissiveColor", {0.0f, 0.0f, 0.0f, 1.0f});
	DynamicMaterial->SetScalarParameterValue("EmissiveCoeff", 0.0f);
	LampMesh->SetMaterial(0, DynamicMaterial);
}


void UG2ISliderLampComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	if (LampMesh)
	{
		LampMesh->DestroyComponent();
		LampMesh = nullptr;
	}
}

void UG2ISliderLampComponent::ChangeIntensity(int IntensityChangeDir, float TargetLightIntensity)
{
	float EmissiveCoeff = DynamicMaterial->K2_GetScalarParameterValue("EmissiveCoeff");
	if (IntensityChangeDir == 1)
	{
		DynamicMaterial->SetScalarParameterValue("EmissiveCoeff", FMath::Min(TargetLightIntensity, EmissiveCoeff + LightIntensityRate));
	}
	else
	{
		DynamicMaterial->SetScalarParameterValue("EmissiveCoeff", FMath::Max(TargetLightIntensity, EmissiveCoeff - LightIntensityRate));
	}

	if (EmissiveCoeff == TargetLightIntensity)
	{
		GetWorld()->GetTimerManager().ClearTimer(IntensityColorTimer);
		if (FMath::IsNearlyEqual(EmissiveCoeff, 0.0f))
		{
			DynamicMaterial->SetVectorParameterValue("EmissiveColor", {0.0f, 0.0f, 0.0f, 1.0f});
		}
	}
}

void UG2ISliderLampComponent::SetTimerToIntensity(int IntensityChangeDir)
{
	GetWorld()->GetTimerManager().ClearTimer(IntensityColorTimer);
	GetWorld()->GetTimerManager().SetTimer(IntensityColorTimer, [this, IntensityChangeDir]()
	{
		if (LampMode == 2)
		{
			this->ChangeIntensity(IntensityChangeDir, MaxLightIntensityInActivationColorZone);
		}
		else if (LampMode == 1)
		{
			this->ChangeIntensity(IntensityChangeDir, MaxLightIntensityInCommonColorZone);
		}else
		{
			this->ChangeIntensity(IntensityChangeDir, 0);
		}
	}, IntensityIncreaseFrequency, true);
}

void UG2ISliderLampComponent::SetTimerToFlashing(int FlashCount, float FlashTime)
{
	bIsLampFlashing = true;
	//GetWorld()->GetTimerManager().ClearTimer(FlashingTimer);
	GetWorld()->GetTimerManager().SetTimer(FlashingTimer, [this, FlashCount]()
	{
		this->LampFlashing(FlashCount);
	}, FlashTime, true, 0.0f);
}

void UG2ISliderLampComponent::LampFlashing(int FlashCount)
{
	if (FlashCounter < FlashCount)
	{
		if (bLampFlashState)
		{
			DynamicMaterial->SetScalarParameterValue("EmissiveCoeff", 0.0f);
		}
		else
		{
			DynamicMaterial->SetScalarParameterValue("EmissiveCoeff", 100.0f);
		}
		bLampFlashState = !bLampFlashState;
		if (bLampFlashState)
		{
			FlashCounter++;
		}
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(FlashingTimer);
		bIsLampFlashing = false;
		FlashCounter = 0;
		if (LampMode == 0)
		{
			SetTimerToIntensity(-1);
		}
		if (LampMode == 1)
		{
			SetTimerToIntensity(-1);
		}
	}
}

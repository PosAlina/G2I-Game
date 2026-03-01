


#include "Gameplay/G2ISliderLampComponent.h"

#include "G2I.h"
#include "BaseGizmos/GizmoElementRenderState.h"
#include "Components/PointLightComponent.h"

UG2ISliderLampComponent::UG2ISliderLampComponent()
{
	LampMesh = CreateDefaultSubobject<UStaticMeshComponent>("LampMesh");
	
	if (!ensure(LampMesh))
	{
		UE_LOG(LogG2I, Error, TEXT("LampMesh was not created in %s"), *GetName());
		return;
	}
	
	LampMesh->SetupAttachment(this);
}

void UG2ISliderLampComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (LightMaterial)
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(LightMaterial, this);
	}

	if (!DynamicMaterial)
	{
		UE_LOG(LogG2I, Warning, TEXT("Dynamic material was not created in %s"), *GetName());
		return;
	}
	
	DynamicMaterial->SetVectorParameterValue("BaseColor", LampColor);
	DynamicMaterial->SetVectorParameterValue("EmissiveColor", {0.0f, 0.0f, 0.0f, 1.0f});
	DynamicMaterial->SetScalarParameterValue("EmissiveCoeff", 0.0f);
	
	if (LampMesh)
	{
		LampMesh->SetMaterial(0, DynamicMaterial);
	}
}


void UG2ISliderLampComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);
	if (LampMesh)
	{
		LampMesh->DestroyComponent();
		LampMesh = nullptr;
	}
}

void UG2ISliderLampComponent::ChangeIntensity(int IntensityChangeDir, float TargetLightIntensity)
{
	if (DynamicMaterial)
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
			if (!ensure(GetWorld()))
			{
				UE_LOG(LogG2I, Warning, TEXT("World is null in %s"), *GetName());
				return;
			}
			
			GetWorld()->GetTimerManager().ClearTimer(IntensityColorTimer);
			if (FMath::IsNearlyEqual(EmissiveCoeff, 0.0f))
			{
				//DynamicMaterial->
				DynamicMaterial->SetVectorParameterValue("EmissiveColor", {0.0f, 0.0f, 0.0f, 1.0f});
			}
		}
	}	
}

void UG2ISliderLampComponent::SetTimerToIntensity(int IntensityChangeDir)
{
	if (!ensure(GetWorld()))
	{
		UE_LOG(LogG2I, Warning, TEXT("World is null in %s"), *GetName());
		return;
	}
	
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
		}
		else
		{
			this->ChangeIntensity(IntensityChangeDir, 0);
		}
	}, IntensityIncreaseFrequency, true);
}

void UG2ISliderLampComponent::SetTimerToFlashing(int FlashCount, float FlashTime)
{
	bIsLampFlashing = true;
	if (!ensure(GetWorld()))
	{
		UE_LOG(LogG2I, Warning, TEXT("World is null in %s"), *GetName());
		return;
	}
	
	GetWorld()->GetTimerManager().SetTimer(FlashingTimer, [this, FlashCount]()
	{
		LampFlashing(FlashCount);
	}, FlashTime, true, 0.0f);
}

void UG2ISliderLampComponent::LampFlashing(int FlashCount)
{
	if (FlashCounter < FlashCount)
	{
		if (DynamicMaterial)
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
	}
	else
	{
		if (!ensure(GetWorld()))
		{
			UE_LOG(LogG2I, Warning, TEXT("World is null in %s"), *GetName());
			return;
		}
		
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

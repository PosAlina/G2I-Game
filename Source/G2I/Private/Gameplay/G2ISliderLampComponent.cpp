#include "Gameplay/G2ISliderLampComponent.h"
#include "G2I.h"
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
	
	PointLightComp = CreateDefaultSubobject<UPointLightComponent>("PointLight");
	if (!ensure(PointLightComp))
	{
		UE_LOG(LogG2I, Error, TEXT("PointLight was not created in %s"), *GetName());
		return;
	}
	PointLightComp->SetupAttachment(LampMesh);
}

void UG2ISliderLampComponent::BeginPlay()
{
	Super::BeginPlay();

	SetupDefaults();
}

void UG2ISliderLampComponent::SetupDefaults()
{
	World = GetWorld();
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(), *UWorld::StaticClass()->GetName());
	}
	if (!ensure(LampMesh))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find LampMesh"), *GetName());
		return;
	}

	DynamicMaterial = LampMesh->CreateAndSetMaterialInstanceDynamic(0);
	if (!DynamicMaterial)
	{
		UE_LOG(LogG2I, Log, TEXT("%s: LampMesh doesn't have material"), *GetName());
		DynamicMaterial = UMaterialInstanceDynamic::Create(LightMaterial, this);
	}
	if (!ensure(DynamicMaterial))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Dynamic material was not created"), *GetName());
		return;
	}
	SetDefaultValues();
}

void UG2ISliderLampComponent::SetDefaultValues()
{
	if (!ensure(PointLightComp))
	{
		UE_LOG(LogG2I, Error, TEXT("PointLight was not created in %s"), *GetName());
	}
	else
	{
		PointLightComp->SetVisibility(EmissiveInfo.bIsPointLightEnabled);
	}
	
	if (BaseColor != FLinearColor::Transparent)
	{
		SetBaseColor(BaseColor);
	}
	
	SetEmissiveColor(EmissiveInfo.Color);
	
	if (EmissiveInfo.bIsOn)
	{
		OnLamp();
	}
	else
	{
		OffLamp();
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

void UG2ISliderLampComponent::ChangeIntensity(const int32 IntensityChangeDir, const float TargetLightIntensity)
{
	if (DynamicMaterial)
	{
		const float OldEmissiveIntensity = CurrentEmissiveIntensity;
		if (IntensityChangeDir == 1)
		{
			SetCurrentEmissiveIntensity(FMath::Min(TargetLightIntensity, OldEmissiveIntensity + EmissiveInfo.IntensityRate));
		}
		else
		{
			SetCurrentEmissiveIntensity(FMath::Max(TargetLightIntensity, OldEmissiveIntensity - EmissiveInfo.IntensityRate));
		}

		if (OldEmissiveIntensity == TargetLightIntensity)
		{
			StopTimerToIntensity();
			if (FMath::IsNearlyEqual(OldEmissiveIntensity, 0.f))
			{
				OffLamp();
			}
		}
	}	
}

void UG2ISliderLampComponent::SetTimerToIntensity(const int32 IntensityChangeDir)
{
	StopTimerToIntensity();

	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(), *UWorld::StaticClass()->GetName());
		return;
	}
	
	World->GetTimerManager().SetTimer(IntensityColorTimer, [this, IntensityChangeDir]()
	{
		if (LampMode == 2)
		{
			ChangeIntensity(IntensityChangeDir, MaxLightIntensityInActivationColorZone);
		}
		else if (LampMode == 1)
		{
			ChangeIntensity(IntensityChangeDir, MaxLightIntensityInCommonColorZone);
		}
		else
		{
			ChangeIntensity(IntensityChangeDir, 0);
		}
	}, IntensityIncreaseFrequency, true);
}

void UG2ISliderLampComponent::StopTimerToIntensity()
{
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(), *UWorld::StaticClass()->GetName());
		return;
	}
	
	World->GetTimerManager().ClearTimer(IntensityColorTimer);
}

void UG2ISliderLampComponent::SetTimerToFlashing(const float FlashTime, uint32 FlashCount)
{
	if (bIsLampFlashing)
	{
		StopTimerToFlashing();
	}

	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(), *UWorld::StaticClass()->GetName());
		return;
	}
	bIsLampFlashing = true;
	
	World->GetTimerManager().SetTimer(FlashingTimer, [this, FlashCount]()
	{
		LampFlashing(FlashCount);
	}, FlashTime, true, 0.0f);
}

void UG2ISliderLampComponent::SetTimerToFlashing(const float FlashTime)
{
	if (bIsLampFlashing)
	{
		StopTimerToFlashing();
	}

	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(), *UWorld::StaticClass()->GetName());
		return;
	}
	bIsLampFlashing = true;
	
	World->GetTimerManager().SetTimer(FlashingTimer, [this]()
	{
		LampFlashing();
	}, FlashTime, true, 0.0f);
	return;
}

void UG2ISliderLampComponent::StopTimerToFlashing()
{
	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(), *UWorld::StaticClass()->GetName());
		return;
	}
	World->GetTimerManager().ClearTimer(FlashingTimer);
	bIsLampFlashing = false;
	if (OnStopFlashingTimer.IsBound())
	{
		OnStopFlashingTimer.Execute();
	}
}

void UG2ISliderLampComponent::SetBaseColor(const FLinearColor& NewBaseColor)
{
	BaseColor = NewBaseColor;

	if (!ensure(DynamicMaterial))
	{
		return;
	}
	DynamicMaterial->SetVectorParameterValue("Base Color", NewBaseColor);
}

void UG2ISliderLampComponent::SetEmissiveColor(const FLinearColor& NewEmissiveColor)
{
	EmissiveInfo.Color = NewEmissiveColor;

	if (DynamicMaterial)
	{
		DynamicMaterial->SetVectorParameterValue("Emissive Color", NewEmissiveColor);
	}
	if (PointLightComp)
	{
		PointLightComp->SetLightColor(NewEmissiveColor);
	}
}

void UG2ISliderLampComponent::SetCurrentEmissiveIntensity(const float NewEmissiveIntensity)
{
	CurrentEmissiveIntensity = NewEmissiveIntensity;

	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue("Emissive Intensity", NewEmissiveIntensity);
	}
	if (PointLightComp)
	{
		PointLightComp->SetIntensity(NewEmissiveIntensity * EmissiveInfo.MultiplePointLightIntensity);
	}
}

void UG2ISliderLampComponent::SetMaxEmissiveIntensity(const float NewEmissiveIntensity)
{
	EmissiveInfo.MaxIntensity = NewEmissiveIntensity;
}

void UG2ISliderLampComponent::OnLamp()
{
	StopTimerToFlashing();
	StopTimerToIntensity();
	EmissiveInfo.bIsOn = true;
	SetCurrentEmissiveIntensity(EmissiveInfo.MaxIntensity);
	bLampFlashState = true;
}

void UG2ISliderLampComponent::OffLamp()
{
	StopTimerToFlashing();
	StopTimerToIntensity();
	EmissiveInfo.bIsOn = false;
	SetCurrentEmissiveIntensity(0.f);
	bLampFlashState = false;
}

FLinearColor UG2ISliderLampComponent::GetBaseColor() const
{
	return BaseColor;
}

void UG2ISliderLampComponent::LampFlashing(const uint32 FlashCount)
{
	if (FlashCounter < FlashCount)
	{
		LampFlashing();
		if (bLampFlashState)
		{
			FlashCounter++;
		}
	}
	else
	{
		StopTimerToFlashing();
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

void UG2ISliderLampComponent::LampFlashing()
{
	if (!ensure(DynamicMaterial))
	{
		return;
	}
	
	if (bLampFlashState)
	{
		SetCurrentEmissiveIntensity(0.f);
	}
	else
	{
		SetCurrentEmissiveIntensity(EmissiveInfo.MaxIntensity);
	}
	bLampFlashState = !bLampFlashState;
}
#include "Gameplay/G2ISliderLampComponent.h"
#include "G2I.h"
#include "Sound/G2ISoundComponent.h"

UG2ISliderLampComponent::UG2ISliderLampComponent()
{
	LampMesh = CreateDefaultSubobject<UStaticMeshComponent>("LampMesh");
	const FString ComponentName = FString::Printf(TEXT("%s_LampSoundComponent"), *GetName());
	SoundComp = CreateDefaultSubobject<UG2ISoundComponent>(*ComponentName);
	
	if (!ensure(LampMesh))
	{
		UE_LOG(LogG2I, Error, TEXT("LampMesh was not created in %s"), *GetName());
	}
	if (!ensure(SoundComp)) {
		UE_LOG(LogG2I, Error, TEXT("LampSoundComponent was not created in %s"), *GetName());
	}
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
	SetupMaterialDefaults();
	SetupSoundDefaults();
	SetDefaultValues();
}

void UG2ISliderLampComponent::SetupSoundDefaults()
{
	if (!ensure(SoundComp))
	{
		UE_LOG(LogG2I, Error, TEXT("SoundComponent doesn't exist in %s"), *GetName());
		return;
	}
	SoundComp->SetupSounds.Add(CracklingLampSoundName);
	const auto* CracklingLampConf = SoundComp->SetupSounds.Find(CracklingLampSoundName);
	if (!ensure(CracklingLampConf))
	{
		UE_LOG(LogG2I, Warning, TEXT("%s: Couldn't find sound by name %s"),
			*GetName(), *CracklingLampSoundName.ToString());
		return;
	}
	if (CracklingLampConf->Sound) // Exist for slider, optionally for checkers
	{
		CracklingLampSoundId = SoundComp->AddSound(*CracklingLampConf);
		if (!SoundComp->PlaySound(CracklingLampSoundId))
		{
			UE_LOG(LogG2I, Warning, TEXT("%s: Couldn't play sound"), *GetName());
		}
	}
	else
	{
		CracklingLampSoundId = -1;
	}
}

void UG2ISliderLampComponent::SetupMaterialDefaults()
{
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
}

void UG2ISliderLampComponent::SetDefaultValues()
{
	
	if (BaseColor != FLinearColor::Transparent)
	{
		SetBaseColor(BaseColor);
	}
	
	SetEmissiveColor(EmissiveInfo.Color);
	SetMaxEmissiveIntensity(MaxLightIntensityInActivationColorZone);
	
	if (EmissiveInfo.bIsOn)
	{
		OnLamp();
	}
	else
	{
		OffLamp();
	}
}

void UG2ISliderLampComponent::ChangeIntensity(const int32 IntensityChangeDir, const float TargetLightIntensity)
{
	if (!IsValid(DynamicMaterial))
	{
		return;
	}
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

void UG2ISliderLampComponent::SetTimerToIntensity(const int32 IntensityChangeDir)
{
	StopTimerToIntensity();

	if (!ensure(World))
	{
		UE_LOG(LogG2I, Error, TEXT("%s: Couldn't find %s"), *GetName(), *UWorld::StaticClass()->GetName());
		return;
	}

	const FTimerDelegate Delegate = FTimerDelegate::CreateUObject( this, &ThisClass::ChangeIntensity, IntensityChangeDir);
	World->GetTimerManager().SetTimer(FlashingTimer, Delegate, IntensityIncreaseFrequency, true);
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

	const FTimerDelegate Delegate = FTimerDelegate::CreateUObject( this, &ThisClass::LampFlashing, FlashCount);
	World->GetTimerManager().SetTimer(FlashingTimer, Delegate, FlashTime, true);
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

	World->GetTimerManager().SetTimer(FlashingTimer, this, &ThisClass::LampFlashing, FlashTime, true);
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

	if (!IsValid(DynamicMaterial))
	{
		return;
	}
	FLinearColor OutValue;
	if (!ensure(DynamicMaterial->GetVectorParameterValue(FName("Base Color"), OutValue)))
	{
		G2I::DebugWarningMessage("Material of " + GetName() + " has no Base Color" );
		return;
	}
	DynamicMaterial->SetVectorParameterValue("Base Color", NewBaseColor);
}

void UG2ISliderLampComponent::SetEmissiveColor(const FLinearColor& NewEmissiveColor)
{
	EmissiveInfo.Color = NewEmissiveColor;

	if (!IsValid(DynamicMaterial))
	{
		return;
	}
	FLinearColor OutValue;
	if (!ensure(DynamicMaterial->GetVectorParameterValue(FName("Emissive Color"), OutValue)))
	{
		G2I::DebugWarningMessage("Material of " + GetName() + " has no Emissive Color" );
	}
	else
	{
		DynamicMaterial->SetVectorParameterValue("Emissive Color", NewEmissiveColor);
	}
}

void UG2ISliderLampComponent::SetCurrentEmissiveIntensity(const float NewEmissiveIntensity)
{
	CurrentEmissiveIntensity = NewEmissiveIntensity;

	if (!IsValid(DynamicMaterial))
	{
		return;
	}

	float OutValue;
	if (!ensure(DynamicMaterial->GetScalarParameterValue(FName("Emissive Intensity"), OutValue)))
	{
		G2I::DebugWarningMessage("Material of " + GetName() + " has no Emissive Intensity" );
	}
	else
	{
		DynamicMaterial->SetScalarParameterValue("Emissive Intensity", NewEmissiveIntensity);
	}
	
	if (!ensure(SoundComp))
	{
		UE_LOG(LogG2I, Error, TEXT("SoundComponent doesn't exist in %s"), *GetName());
		return;
	}
	if (CracklingLampSoundId != -1)
	{
		if (!SoundComp->SetSoundVolume(CracklingLampSoundId, NewEmissiveIntensity))
		{
			UE_LOG(LogG2I, Warning, TEXT("%s: Couldn't set sound with ID %i"), *GetName(), CracklingLampSoundId);
			return;
		}
	}
}

void UG2ISliderLampComponent::SetMaxEmissiveIntensity(const float NewEmissiveIntensity)
{
	EmissiveInfo.MaxIntensity = NewEmissiveIntensity;
}

void UG2ISliderLampComponent::SetLampEmissiveInfo(const FG2ILampEmissiveInfo& NewLampEmissiveInfo)
{
	SetEmissiveColor(NewLampEmissiveInfo.Color);
	SetCurrentEmissiveIntensity(NewLampEmissiveInfo.IntensityRate);
	SetMaxEmissiveIntensity(NewLampEmissiveInfo.MaxIntensity);
	if (NewLampEmissiveInfo.bIsOn)
	{
		OnLamp();
	}
	else
	{
		OffLamp();
	}
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
		switch(LampMode)
		{
		case 0:
			SetTimerToIntensity(-1);
			break;
		case 1:
			SetTimerToIntensity(-1);
			break;
		default: ;
		}
	}
}

void UG2ISliderLampComponent::LampFlashing()
{
	if (!IsValid(DynamicMaterial))
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

void UG2ISliderLampComponent::ChangeIntensity(const int32 IntensityChangeDir)
{
	switch (LampMode)
	{
	case 2:
		ChangeIntensity(IntensityChangeDir, MaxLightIntensityInActivationColorZone);
		break;
	case 1:
		ChangeIntensity(IntensityChangeDir, MaxLightIntensityInCommonColorZone);
		break;
	default:
		ChangeIntensity(IntensityChangeDir, 0);
	}
}

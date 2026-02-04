#pragma once

UENUM(BlueprintType)
enum class EG2IAimType : uint8
{
	DefaultAim UMETA(DisplayName = "Default aim"),
	TargetAim UMETA(DisplayName = "Aim looking at the target actor"),
	SuccessfulHitAim UMETA(DisplayName = "Aim after successful hit"),
	MissedHitAim UMETA(DisplayName = "Aim after missed hit")
};
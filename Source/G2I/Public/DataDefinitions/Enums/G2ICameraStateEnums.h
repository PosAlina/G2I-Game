#pragma once

UENUM(BlueprintType)
enum class EG2ICameraTypeEnum : uint8
{
	ThirdPersonCamera UMETA(DisplayName = "Third-Person Camera"),
	FixedCamera UMETA(DisplayName = "Fixed Camera")
};

UENUM(BlueprintType)
enum class EG2ICameraBlendState : uint8
{
	Start UMETA(DisplayName = "Start blend transition of camera"),
	Finish UMETA(DisplayName = "Finish blend transition of camera")
};
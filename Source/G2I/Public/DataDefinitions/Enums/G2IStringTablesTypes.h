#pragma once

UENUM(BlueprintType)
enum class EG2IStringTablesTypes : uint8
{
	CommonUI UMETA(DisplayName = "String table of common UI"),
	Confirmations UMETA(DisplayName = "String table of confirmations widgets"),
	Options UMETA(DisplayName = "String table of options widget"),
	Creators UMETA(DisplayName = "String table of creators widget"),
	Controls UMETA(DisplayName = "String table of controls widget"),
	TrainingScreen UMETA(DisplayName = "String table of debug HUD with mechanic description")
};
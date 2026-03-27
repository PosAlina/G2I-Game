// Copyright Epic Games, Inc. All Rights Reserved.

#include "G2I.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, G2I, "G2I" );

DEFINE_LOG_CATEGORY(LogG2I)

void G2I::DebugErrorMessage(const FString& DebugMessage, const FColor& Color, const float Time)
{
	UE_LOG(LogG2I, Warning, TEXT("%s"), *DebugMessage);
#if WITH_EDITOR
	ScreenEditorMessage(DebugMessage, Color, Time);
#endif
}

void G2I::DebugWarningMessage(const FString& DebugMessage, const FColor& Color, const float Time)
{
	UE_LOG(LogG2I, Warning, TEXT("%s"), *DebugMessage);
#if WITH_EDITOR
	ScreenEditorMessage(DebugMessage, Color, Time);
#endif
}

void G2I::DebugLogMessage(const FString& DebugMessage, const FColor& Color, const float Time)
{
	UE_LOG(LogG2I, Log, TEXT("%s"), *DebugMessage);
#if WITH_EDITOR
	ScreenEditorMessage(DebugMessage, Color, Time);
#endif
}

#if WITH_EDITOR
void G2I::ScreenEditorMessage(const FString& DebugMessage, const FColor& Color, const float Time)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Time, Color, DebugMessage);
	}
}
#endif

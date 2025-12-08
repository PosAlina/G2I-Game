// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class G2I : ModuleRules
{
	public G2I(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"AIModule",
			"CinematicCamera"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"G2I",
			"G2I/Public",
			"G2I/Public/AI",
			"G2I/Public/Controls",
			"G2I/Public/Characters",
			"G2I/Public/DataDefinitions",
			"G2I/Public/Game",
			"G2I/Public/Gameplay",
			"G2I/Public/Interfaces",
			"G2I/Public/UI"
        });
		
		PrivateIncludePaths.AddRange(new string[] {
			"G2I",
			"G2I/Private",
			"G2I/Private/AI",
			"G2I/Private/Controls",
			"G2I/Private/Characters",
			"G2I/Private/DataDefinitions",
			"G2I/Private/Game",
			"G2I/Private/Gameplay",
			"G2I/Private/Interfaces",
			"G2I/Private/UI"
        });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

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
      "GameplayTags",
			"CinematicCamera",
			"NavigationSystem"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"G2I",
			"G2I/Public",
			"G2I/Public/AI",
			"G2I/Public/AI/Navigation",
			"G2I/Public/Controls",
			"G2I/Public/Characters",
			"G2I/Public/DataDefinitions",
			"G2I/Public/DataDefinitions/DataTables",
			"G2I/Public/DataDefinitions/Enums",
			"G2I/Public/DataDefinitions/Structs",
			"G2I/Public/Game",
			"G2I/Public/Gameplay",
			"G2I/Public/Interfaces",
			"G2I/Public/Interfaces/Camera",
			"G2I/Public/Interfaces/SteamGlove",
			"G2I/Public/UI",
			"G2I/Public/UI/Widgets",
			"G2I/Public/UI/WidgetComponents",
			"G2I/Public/Components",
			"G2I/Public/Components/Camera",
			"G2I/Public/Components/SteamGlove"
        });
		
		PrivateIncludePaths.AddRange(new string[] {
			"G2I",
			"G2I/Private",
			"G2I/Private/AI",
			"G2I/Private/AI/Navigation",
			"G2I/Private/Controls",
			"G2I/Private/Characters",
			"G2I/Private/DataDefinitions",
			"G2I/Private/DataDefinitions/DataTables",
			"G2I/Private/DataDefinitions/Enums",
			"G2I/Private/DataDefinitions/Structs",
			"G2I/Private/Game",
			"G2I/Private/Gameplay",
			"G2I/Private/Interfaces",
			"G2I/Private/Interfaces/Camera",
			"G2I/Private/Interfaces/SteamGlove",
			"G2I/Private/UI",
			"G2I/Private/UI/Widgets",
			"G2I/Private/UI/WidgetComponents",
			"G2I/Private/Components",
			"G2I/Private/Components/Camera",
			"G2I/Private/Components/SteamGlove"
        });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

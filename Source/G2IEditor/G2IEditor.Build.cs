using UnrealBuildTool;

public class G2IEditor : ModuleRules
{
    public G2IEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
        [
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "G2I",
            "DetailCustomizations",
            "Slate",
            "SlateCore",
            "UnrealEd",
            "PropertyEditor",
            "EditorStyle"
        ]);

        PrivateDependencyModuleNames.AddRange([]);

        PublicIncludePaths.AddRange([
            "G2IEditor",
            "G2IEditor/Public"
        ]);

        PrivateIncludePaths.AddRange([
            "G2IEditor",
            "G2IEditor/Private"
        ]);
    }
}
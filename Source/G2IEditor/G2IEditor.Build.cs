using UnrealBuildTool;

public class G2IEditor : ModuleRules
{
    public G2IEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
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
        });
    }
}
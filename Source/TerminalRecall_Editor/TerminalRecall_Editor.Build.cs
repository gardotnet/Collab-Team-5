using UnrealBuildTool;

public class TerminalRecall_Editor : ModuleRules
{
	public TerminalRecall_Editor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"UMG", 
			"Terminal_Recall",
			"Slate",        // Added for FSlateApplication
			"SlateCore"     // Added for ESelectInfo / FComboBoxStyle
		});

		PrivateDependencyModuleNames.AddRange(new string[] { 
			"UnrealEd", 
			"Blutility", 
			"UMGEditor",
			"Json",                // Added for JsonObjectConverter
			"JsonUtilities",       // Added for JsonObjectConverter
			"DesktopPlatform",      // Added for IDesktopPlatform
			"LevelEditor"
		});
	}
}
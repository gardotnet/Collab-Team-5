// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Terminal_Recall : ModuleRules
{
	public Terminal_Recall(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "Slate", "SlateCore", "Json", "JsonUtilities", "DesktopPlatform" });

		/*if (Target.bBuildEditor)
		{
			PublicDependencyModuleNames.AddRange(new string[] { "Blutility", "UMGEditor" });
			
			PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd", "EditorFramework", "LevelEditor" });
		}*/

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Soldier : ModuleRules
{
	public Soldier(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", 
			"InputCore", "HeadMountedDisplay", "UMG", "NavigationSystem"});
	}
}

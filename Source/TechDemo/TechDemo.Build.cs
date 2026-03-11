// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TechDemo : ModuleRules
{
	public TechDemo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "AIModule", "NavigationSystem", "GameplayTasks", "UMG"
			, "GameplayAbilities", "GameplayTags"
        });
    }
}

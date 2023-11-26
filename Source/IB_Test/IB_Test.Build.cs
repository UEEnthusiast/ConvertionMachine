// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class IB_Test : ModuleRules
{
	public IB_Test(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "DeveloperSettings" });
	}
}

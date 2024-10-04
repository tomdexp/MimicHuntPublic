// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MimicHunt : ModuleRules
{
	public MimicHunt(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { });
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"OnlineSubsystem",
			"OnlineSubsystemEOS",
			"OnlineSubsystemUtils",
			"UE5Coro"
		});
		PrivateDefinitions.Add("P2PMODE=1");
	}
}

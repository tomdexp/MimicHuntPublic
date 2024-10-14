// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MimicHunt : ModuleRules
{
	public MimicHunt(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] {});
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"OnlineSubsystemEIK",
			"OnlineSubsystemUtils",
			"ControlRig",
			"RigVM",
			"UE5Coro",
			"TencentGME_Wwise",
			"Wwise",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"UMG"
		});
	}
}

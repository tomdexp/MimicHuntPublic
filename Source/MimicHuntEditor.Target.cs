// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class MimicHuntEditorTarget : TargetRules
{
	public MimicHuntEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
		bUseUnityBuild = false;
		ExtraModuleNames.Add("MimicHunt");
	}
}

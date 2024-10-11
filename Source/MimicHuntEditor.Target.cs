using UnrealBuildTool;
using System.Collections.Generic;

public class MimicHuntEditorTarget : TargetRules
{
	public MimicHuntEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		bUseUnityBuild = false;
		ExtraModuleNames.Add("MimicHunt");
	}
}

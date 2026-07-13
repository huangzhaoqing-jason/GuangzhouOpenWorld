using UnrealBuildTool;
using System.Collections.Generic;

public class GuangzhouOpenWorldEditorTarget : TargetRules
{
	public GuangzhouOpenWorldEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_9;
		CppStandard = CppStandardVersion.Cpp20;

		ExtraModuleNames.Add("GuangzhouOpenWorld");

		if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			bCompileAgainstEngine = true;
			bCompileWithPluginSupport = true;
			bUseAppleOS = true;
		}
	}
}
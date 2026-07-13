using UnrealBuildTool;
using System.Collections.Generic;

public class GuangzhouOpenWorldTarget : TargetRules
{
	public GuangzhouOpenWorldTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_9;
		CppStandard = CppStandardVersion.Cpp20;

		ExtraModuleNames.Add("GuangzhouOpenWorld");

		bUseLoggingInShipping = true;
		bUseLauncherChecks = false;
		bUseChecksInShipping = false;

		if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			bCompileAgainstEngine = true;
			bCompileWithPluginSupport = true;
			bUseAppleOS = true;
			bBuildDeveloperTools = true;
		}
	}

	public override void ModifyBuildEnvironment(TargetInfo Target, BuildEnvironmentSettings OutEnv)
	{
		base.ModifyBuildEnvironment(Target, OutEnv);

		if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			OutEnv.Definitions.Add("METAL_4_2=1");
			OutEnv.Definitions.Add("APPLE_SILICON_UMA=1");
			OutEnv.Definitions.Add("SOLOUD_ENABLED=1");
			OutEnv.Definitions.Add("JOLT_PHYSICS_V6=1");

			OutEnv.AdditionalCompilerArguments += " -march=armv8.6-a+fp+simd+crypto+rcpc+neon";
			OutEnv.AdditionalCompilerArguments += " -O3";
			OutEnv.AdditionalCompilerArguments += " -flto";
			OutEnv.LinkerArguments += " -Wl,-dead_strip";
		}
	}
}
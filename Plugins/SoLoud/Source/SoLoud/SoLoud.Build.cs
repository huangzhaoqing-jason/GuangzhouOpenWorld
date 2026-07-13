using UnrealBuildTool;
using System.IO;

public class SoLoud : ModuleRules
{
	public SoLoud(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp20;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"AudioMixer",
			"AudioMixerCore",
			"Projects",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"CoreUObject",
			"Engine",
		});

		PublicDefinitions.Add("SOLOUD_VERSION_2026=1");
		PublicDefinitions.Add("WITH_SOLOUD=1");
		PublicDefinitions.Add("SOLOUD_COREAUDIO=1");

		if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			PublicDefinitions.Add("SOLOUD_PLATFORM_MACOS=1");
			PublicDefinitions.Add("SOLOUD_ARM64_NEON=1");

			string PluginDir = Path.Combine(ModuleDirectory, "../..");
			string LibDir = Path.Combine(PluginDir, "Lib", "Mac");
			PublicAdditionalLibraries.Add(Path.Combine(LibDir, "libSoLoud.a"));
			PublicFrameworks.AddRange(new string[] { "CoreAudio", "AudioToolbox", "AVFoundation" });
			PublicIncludePaths.Add(Path.Combine(PluginDir, "Source", "ThirdParty", "SoLoud"));
		}
		else
		{
			throw new BuildException("SoLoud plugin only supports macOS Apple Silicon in this project.");
		}

		OptimizeCode = CodeOptimization.InShippingBuildsOnly;
		bUseUnity = true;
	}
}

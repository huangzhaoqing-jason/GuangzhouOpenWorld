using UnrealBuildTool;
using System.IO;

public class JoltPhysics : ModuleRules
{
	public JoltPhysics(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp20;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"PhysicsCore",
			"Projects",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"CoreUObject",
			"Engine",
		});

		PublicDefinitions.Add("JOLT_VERSION_601=1");
		PublicDefinitions.Add("JPH_NO_DEBUG");
		PublicDefinitions.Add("JPH_DISABLE_CUSTOM_ALLOCATOR");
		PublicDefinitions.Add("JPH_ENABLE_ASSERTS=0");

		if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			PublicDefinitions.Add("JPH_PLATFORM_MACOS=1");
			PublicDefinitions.Add("JPH_USE_NEON=1");
			PublicDefinitions.Add("JPH_CPU_ADDRESS_BITS=64");
			PublicDefinitions.Add("JPH_OBJECT_LAYER_BITS=16");

			string PluginDir = Path.Combine(ModuleDirectory, "../..");
			string LibDir = Path.Combine(PluginDir, "Lib", "Mac");
			PublicAdditionalLibraries.Add(Path.Combine(LibDir, "libJolt.a"));
			PublicIncludePaths.Add(Path.Combine(PluginDir, "Source", "ThirdParty", "Jolt"));
		}
		else
		{
			throw new BuildException("JoltPhysics plugin only supports macOS Apple Silicon in this project.");
		}

		OptimizeCode = CodeOptimization.InShippingBuildsOnly;
		bUseUnity = true;
	}
}

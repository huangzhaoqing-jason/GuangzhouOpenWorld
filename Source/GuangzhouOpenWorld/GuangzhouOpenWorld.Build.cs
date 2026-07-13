using UnrealBuildTool;

public class GuangzhouOpenWorld : ModuleRules
{
	public GuangzhouOpenWorld(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp20;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"NavigationSystem",
			"Navmesh",
			"AIModule",
			"StateTreeModule",
			"MassEntity",
			"MassAI",
			"MassCommon",
			"MassCrowd",
			"MassMovement",
			"MassNavigation",
			"MassTraffic",
			"MassRepresentation",
			"MassReplication",
			"MassLOD",
			"MassSimulation",
			"MassSpawner",
			"ZoneGraph",
			"WorldPartitionHLOD",
			"Water",
			"Landscape",
			"Foliage",
			"PCG",
			"GeoReferencing",
			"ProceduralMeshComponent",
			"OnlineSubsystem",
			"OnlineSubsystemEOS",
			"EOSSDK",
			"AntiCheatCore",
			"EasyAntiCheat",
			"NetCore",
			"RenderCore",
			"Renderer",
			"RHI",
			"MetalRHI",
			"AudioMixer",
			"AudioMixerCore",
			"SignalProcessing",
			"Slate",
			"SlateCore",
			"UMG",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"Niagara",
			"Chaos",
			"PhysicsCore",
			"AnimGraphRuntime",
			"MoviePlayer",
			"ApplicationCore",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"SoLoud",
			"DeveloperSettings",
		});

		if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			PublicIncludePaths.AddRange(new string[]
			{
				"Source/GuangzhouOpenWorld/Game",
				"Source/GuangzhouOpenWorld/Game/AccountLogin",
				"Source/GuangzhouOpenWorld/Game/GameMainMenu",
				"Source/GuangzhouOpenWorld/Game/SystemSettings",
				"Source/GuangzhouOpenWorld/Physics/JoltPhysicsModule",
				"Source/GuangzhouOpenWorld/AI/RecastMassAI",
				"Source/GuangzhouOpenWorld/Scene/RenderingSystem",
				"Source/GuangzhouOpenWorld/Audio/SoLoudSystem",
				"Source/GuangzhouOpenWorld/Network/EOSSystem",
			});

			PrivateIncludePaths.AddRange(new string[]
			{
				"Source/GuangzhouOpenWorld/Game",
				"Source/GuangzhouOpenWorld/Game/AccountLogin",
				"Source/GuangzhouOpenWorld/Game/GameMainMenu",
				"Source/GuangzhouOpenWorld/Game/SystemSettings",
				"Source/GuangzhouOpenWorld/Physics/JoltPhysicsModule",
				"Source/GuangzhouOpenWorld/AI/RecastMassAI",
				"Source/GuangzhouOpenWorld/Scene/RenderingSystem",
				"Source/GuangzhouOpenWorld/Audio/SoLoudSystem",
				"Source/GuangzhouOpenWorld/Network/EOSSystem",
			});

			PublicFrameworks.AddRange(new string[]
			{
				"Metal",
				"MetalKit",
				"CoreAudio",
				"AudioToolbox",
				"GameController",
				"CoreHaptics",
			});

			PublicDefinitions.Add("METAL_4_2=1");
			PublicDefinitions.Add("APPLE_SILICON_UMA=1");
			PublicDefinitions.Add("SOLOUD_2_10_0=1");
			PublicDefinitions.Add("JOLT_6_0_1=1");
			PublicDefinitions.Add("UE5_9_2=1");
		}

		OptimizeCode = CodeOptimization.InShippingBuildsOnly;
	}
}
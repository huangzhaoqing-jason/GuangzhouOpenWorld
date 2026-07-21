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
			"NetCore",
			"RenderCore",
			"Renderer",
			"RHI",
			"CoreAudio",
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
			"DeveloperSettings",
			"Json",
			"JsonUtilities",
		});

		PublicIncludePaths.AddRange(new string[]
		{
			"Game",
			"Game/Gameplay",
			"Game/MCPSelfCheck",
			"Scene/GSRendering",
			"Editor/GSAssetPipeline",
			"Bridge",
		});

		if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			PublicIncludePaths.AddRange(new string[]
			{
				"Game",
				"Game/AccountLogin",
				"Game/GameMainMenu",
				"Game/SystemSettings",
				"Game/MCPSelfCheck",
				"Game/Gameplay",
				"Physics/JoltPhysicsModule",
				"Physics/PhysicsWind",
				"AI/RecastMassAI",
				"Scene/RenderingSystem",
				"Audio/SoLoudSystem",
				"Network/EOSSystem",
				"Bridge",
			});

			PrivateIncludePaths.AddRange(new string[]
			{
				"Game",
				"Game/AccountLogin",
				"Game/GameMainMenu",
				"Game/SystemSettings",
				"Game/MCPSelfCheck",
				"Game/Gameplay",
				"Physics/JoltPhysicsModule",
				"Physics/PhysicsWind",
				"AI/RecastMassAI",
				"Scene/RenderingSystem",
				"Audio/SoLoudSystem",
				"Network/EOSSystem",
				"Bridge",
			});

			PublicDependencyModuleNames.AddRange(new string[]
			{
				"MetalRHI",
			});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"SoLoud",
			"JoltPhysics",
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

		PublicDefinitions.Add("METAL_4_3=1");
		PublicDefinitions.Add("APPLE_SILICON_UMA=1");
		PublicDefinitions.Add("SOLOUD_2026=1");
		PublicDefinitions.Add("JOLT_6_0_1=1");
		PublicDefinitions.Add("UE5_8=1");
		}

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[]
			{
				"UnrealEd",
				"AssetTools",
				"EditorStyle",
			});
		}

		OptimizeCode = CodeOptimization.InShippingBuildsOnly;
	}
}
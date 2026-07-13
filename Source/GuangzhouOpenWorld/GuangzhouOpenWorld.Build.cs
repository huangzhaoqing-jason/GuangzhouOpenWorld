// GuangzhouOpenWorld.Build.cs
// 3A开放世界 — 模块依赖配置

using UnrealBuildTool;

public class GuangzhouOpenWorld : ModuleRules
{
    public GuangzhouOpenWorld(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp20;

        // Apple Silicon optimization flags
        if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            bUseRTTI = true;
            bEnableExceptions = true;
            OptimizeCode = CodeOptimization.InShippingBuildsOnly;
        }

        PublicDependencyModuleNames.AddRange(new string[] {
            // Core
            "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
            // Rendering
            "RenderCore", "Renderer", "RHI",
            // Physics
            "PhysicsCore", "Chaos",
            // AI & Navigation
            "AIModule", "NavigationSystem", "MassEntity", "MassAI",
            "MassCrowd", "MassTraffic", "MassGameplay",
            // World
            "WorldPartition", "Landscape", "Water",
            // Audio
            "AudioMixer", "AudioCapture",
            // Network
            "OnlineSubsystem", "OnlineSubsystemUtils",
            // UI
            "UMG", "Slate", "SlateCore",
            // Niagara VFX
            "Niagara", "NiagaraCore",
            // PCG
            "PCG", "StructUtils",
            // Procedural
            "ProceduralMeshComponent",
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "ApplicationCore",
        });

        // Mac-specific
        if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            PublicFrameworks.AddRange(new string[] {
                "Metal", "MetalKit", "CoreAudio", "AudioToolbox",
                "GameController", "CoreHaptics",
            });
        }

        // Include paths
        PublicIncludePaths.AddRange(new string[] {
            "GuangzhouOpenWorld/Game",
            "GuangzhouOpenWorld/Physics",
            "GuangzhouOpenWorld/AI",
            "GuangzhouOpenWorld/Scene",
            "GuangzhouOpenWorld/Audio",
            "GuangzhouOpenWorld/Network",
        });
    }
}
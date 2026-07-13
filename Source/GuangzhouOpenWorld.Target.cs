// GuangzhouOpenWorld.Target.cs
// GTA-广州 — 3A开放世界构建目标

using UnrealBuildTool;
using System.Collections.Generic;

public class GuangzhouOpenWorldTarget : TargetRules
{
    public GuangzhouOpenWorldTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
        ExtraModuleNames.Add("GuangzhouOpenWorld");

        // Apple Silicon Optimization
        if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            bOverrideBuildEnvironment = true;
            // Multi-arch optimization for M1/M2/M3
            AdditionalCompilerArguments += " -mtune=apple-m1 -mcpu=apple-m1 -march=armv8.5-a";
            // Enable Metal 4 backend
            GlobalDefinitions.Add("METAL_4_0=1");
            // Unified Memory Architecture optimization
            GlobalDefinitions.Add("APPLE_SILICON_UMA=1");
            // Enable SoLoud audio backend
            GlobalDefinitions.Add("SOLOUD_ENABLED=1");
        }
    }
}
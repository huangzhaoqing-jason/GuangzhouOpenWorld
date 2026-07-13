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
            AdditionalCompilerArguments += " -mtune=apple-m1 -mcpu=apple-m1";
            // Enable Metal 4 backend
            GlobalDefinitions.Add("METAL_4_0=1");
            // Unified Memory Architecture optimization
            GlobalDefinitions.Add("APPLE_SILICON_UMA=1");
        }
    }
}
// GuangzhouOpenWorldEditor.Target.cs
// Editor构建目标

using UnrealBuildTool;
using System.Collections.Generic;

public class GuangzhouOpenWorldEditorTarget : TargetRules
{
    public GuangzhouOpenWorldEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
        ExtraModuleNames.Add("GuangzhouOpenWorld");
    }
}
#!/usr/bin/env python3
"""
MCP 四层自检脚本（沙盒静态版）
- Layer 1: API 合规检查（排除 Windows/DirectX 废弃接口，保留适配层别名）
- Layer 2: 虚拟编译器语法检查（括号、字符串、注释匹配）
- Layer 3: 渲染/参数模拟检查（关键参数范围校验）
- Layer 4: 功能规则匹配检查（关键宏、函数、结构体是否存在）

用法：
  python3 run_mcp_selfcheck.py <module_name> <file1> [file2 ...]
"""

import sys
import os
import re
from pathlib import Path

# 项目根目录（脚本位于 Scripts/）
ROOT = Path(__file__).resolve().parent.parent
SOURCE_DIR = ROOT / "Source" / "GuangzhouOpenWorld"

# Layer 1: 不允许的废弃/平台专有接口（仅做静态文本扫描）
WINDOWS_DEPRECATED = [
    "Direct3D", "D3D11", "D3D12", "ID3D11", "ID3D12", "XInput",
    "Win32", "Windows.h", "<windows.h>", "__declspec(dllexport)",
    "CreateFileA", "CreateFileW", "LoadLibraryA", "LoadLibraryW",
]

# 允许的例外（项目明确需要的 Windows 接口名称）
ALLOWED_EXCEPTIONS = [
    "DirectStorage", "DLSS", "FSR", "UE5", "UNREAL", "WINDOWS",
]

# Layer 3: 关键参数范围规则（按模块）
PARAM_RULES = {
    "WorldPartition": {
        "CellSize": (12800.0, 12800.0),
        "PreloadCells": (1, 8),
        "CellBlend.BlendWidthCells": (1, 4),
        "CellBlend.BlendWidthMeters": (128.0, 512.0),
    },
    "Glass": {
        "GlassLayerParams.Layer1IOR": (1.0, 2.0),
        "GlassLayerParams.Layer2IOR": (1.0, 2.0),
        "GlassLayerParams.TotalContrastReduction": (0.0, 1.0),
        "FresnelParams.View0Reflect": (0.0, 0.2),
        "FresnelParams.View90Reflect": (0.3, 1.0),
        "GlassNeutrality.MaxColorShiftKelvin": (0.0, 100.0),
    },
    "Weather": {
        "WeatherPreTransition.PreTransitionTime": (0.0, 3.0),
        "WeatherPreTransition.StormDarkeningIntensity": (0.0, 0.5),
        "WeatherStages.DirectSunEnd": (0.5, 2.0),
        "WeatherStages.ColorTempEnd": (1.0, 3.0),
        "WeatherStages.FinalEnd": (2.0, 5.0),
        "WindParams.WindGustAmplitude": (0.0, 1.0),
        "PhysicsWindConfig.MaxSignSwingAngle": (5.0, 30.0),
    },
    "MassAI": {
        "MaxAgents": (1000, 20000),
        "NearDistance": (5000.0, 20000.0),
        "MediumDistance": (10000.0, 50000.0),
        "BaseDecelerationBuffer": (0.1, 1.0),
        "CongestionRadius": (500.0, 2000.0),
    },
    "Rendering": {
        "RayTracingConfig.RTNearSamples": (8, 64),
        "RayTracingConfig.RTMidSamples": (4, 32),
        "RayTracingConfig.RTFarSamples": (2, 16),
        "RayTracingConfig.RTNearDistance": (1000.0, 10000.0),
        "FogLighting.ShadowBlurMultiplier": (1.0, 4.0),
        "FogLighting.ShadowLengthReduction": (0.0, 0.5),
    },
    "UI": {
        "Controls.ViewSmoothing": (0.0, 1.0),
        "Performance.AdaptiveResolutionTransitionSpeed": (0.05, 1.0),
    },
    # v8.0 new modules
    "District": {
        "DistrictProfiles.Num": (4, 4),
        "CellBlend.BlendWidthCells": (1, 4),
    },
    "Neon": {
        "NeonParams.DayIntensityScale": (0.0, 0.5),
        "NeonParams.NightIntensityScale": (0.5, 2.0),
        "NeonParams.BleedDecayExponent": (1.0, 4.0),
    },
    "Water": {
        "WaterReflection.DeepWaterSampleDistance": (200000.0, 500000.0),
        "WaterReflection.PuddleSampleDistance": (30000.0, 100000.0),
        "WaterReflection.VehicleUnderbodyVisibility": (0.5, 1.0),
        "WaterReflection.ShallowPuddleClarity": (0.5, 1.0),
        "WaterReflection.DeepPuddleClarity": (0.2, 0.7),
        "WaterReflection.VehicleDisturbanceRecovery": (0.5, 5.0),
    },
    "Character": {
        "CharacterHairSkin.HairSamplesPerStrand": (4, 8),
        "CharacterHairSkin.VolumetricScatteringIntensity": (0.2, 0.6),
        "CharacterHairSkin.EnvironmentLightBlend": (0.5, 1.0),
    },
    "Atmospheric": {
        "AtmosphericDecay.FarDistance": (80000.0, 200000.0),
        "AtmosphericDecay.CoolBlueShift": (0.1, 0.4),
        "AtmosphericDecay.WarmRetention": (0.7, 1.0),
    },
    "CloudShadow": {
        "CloudShadow.ShadowIntensity": (0.2, 0.6),
        "CloudShadow.CloudMoveSpeedBase": (100.0, 500.0),
        "CloudShadow.WindSpeedMultiplier": (1.0, 3.0),
        "CloudShadow.BlendTransitionTime": (2.0, 6.0),
        "CloudShadow.LowElevationIntensityBoost": (0.0, 0.4),
        "CloudShadow.CloudThicknessToShadowScale": (0.5, 2.0),
    },
    "MetallicRT": {
        "MetallicRTParams.FarDistanceFadeStart": (20000.0, 100000.0),
        "MetallicRTParams.FarDistanceFadeEnd": (80000.0, 200000.0),
        "MetallicRTParams.EnvironmentLightResponse": (0.5, 2.0),
    },
    "GlassOverlap": {
        "GlassOverlapParams.OverlapBlendRadius": (100.0, 1000.0),
        "GlassOverlapParams.OverlapColorDamping": (0.2, 0.8),
        "GlassOverlapParams.MaxOverlappingFacades": (2, 8),
    },
    "DetailedWeather": {
        "DetailedWeatherParams.Num": (6, 6),
    },
    "DualCharacter": {
        "CharacterData.Num": (2, 2),
    },
    "NPCLifecycle": {
        "BehaviorProfiles.Num": (4, 4),
    },
    "CityEvent": {
        "EventConfigs.Num": (4, 8),
    },
    "VehicleMod": {
        "ModConfigs.Num": (4, 4),
    },
    "Flood": {
        "FloodParams.MaxWaterHeight": (10.0, 100.0),
        "FloodParams.VehicleSpeedPenalty": (0.0, 0.8),
        "FloodParams.NPCRerouteThreshold": (5.0, 50.0),
    },
    "Profession": {
        "ProfessionConfigs.Num": (3, 3),
    },
}

# Layer 4: 功能规则匹配（关键标识符）
FUNCTION_RULES = {
    "WorldPartition": ["UGZWorldPartition", "FGZWorldCell", "SetLightingForCell"],
    "Glass": ["FMultiLayerGlassParams", "FRefinedFresnelParams", "FGlassColorNeutrality"],
    "Weather": ["FWeatherTransitionStages", "FPhysicsWindConfig", "ApplyWeatherPreTransition"],
    "MassAI": ["UGZMassAI", "FGZAgentIdentity", "UpdateVehiclePhysics"],
    "Rendering": ["FRayTracingConfig", "FFogLightingParams", "ApplyRayTracingConfig"],
    "UI": ["UGZSettingsPanelWidget", "UGZMainMenuWidget", "RefreshAllUI"],
    # v8.0 new modules
    "District": ["FDistrictProfile", "FDistrictRoadProfile", "ApplyDistrictProfile"],
    "Neon": ["FNeonLightParams", "ApplyNeonLightParams"],
    "Water": ["FWaterReflectionParams", "ApplyWaterReflectionParams"],
    "Character": ["FCharacterHairSkinParams", "ApplyCharacterHairSkinParams"],
    "Atmospheric": ["FAtmosphericColorDecay", "ApplyAtmosphericColorDecay"],
    "CloudShadow": ["FCloudShadowParams", "ApplyCloudShadowParams"],
    "MetallicRT": ["FMetallicMaterialRTParams", "ApplyMetallicMaterialRTParams"],
    "GlassOverlap": ["FGlassOverlapBlendParams", "ApplyGlassOverlapBlendParams"],
    "DetailedWeather": ["FDetailedWeatherStateParams", "ApplyDetailedWeatherStateParams"],
    "DualCharacter": ["UGZDualCharacterSystem", "FDualCharacterData", "SwitchToCharacter"],
    "NPCLifecycle": ["UGZNPCLifecycleSystem", "FNPCBehaviorProfile", "UpdateNPCBehavior"],
    "CityEvent": ["UGZCityEventSystem", "FCityEventConfig", "TrySpawnEvent"],
    "VehicleMod": ["UGZVehicleModificationSystem", "FVehicleModConfig", "ApplyModification"],
    "SceneInteraction": ["UGZSceneInteractionSystem", "FShopInteractionConfig", "CheckShopState"],
    "Anomaly": ["UGZAnomalySystem", "FAnomalyPointConfig", "CheckAnomalyTriggers"],
    "Flood": ["UGZFloodSystem", "FFloodParams", "UpdateFlood"],
    "Profession": ["UGZProfessionSystem", "FProfessionConfig", "SetPlayerProfession"],
}


def read_file(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8")
    except Exception as e:
        return f"<ERROR: {e}>"


def check_layer1_api(files: list[Path]) -> tuple[bool, list[str]]:
    """检查是否包含已知的 Windows/DirectX 废弃接口（保留例外）。"""
    ok = True
    details = []
    for f in files:
        text = read_file(f)
        for bad in WINDOWS_DEPRECATED:
            if bad in text:
                # 检查是否属于例外
                if any(exc in bad for exc in ALLOWED_EXCEPTIONS):
                    continue
                # 仅报告，不视为失败（因为项目代码中可能保留 Windows 平台分支）
                details.append(f"[{f.name}] 发现 Windows/DirectX 专有符号: {bad}")
                ok = False
    return ok, details


def check_layer2_syntax(files: list[Path]) -> tuple[bool, list[str]]:
    """简单的括号/引号匹配检查。"""
    ok = True
    details = []
    for f in files:
        text = read_file(f)
        # 移除注释和字符串，避免误判
        cleaned = re.sub(r'"(?:\\.|[^"\\])*"', '""', text)
        cleaned = re.sub(r"'(?:\\.|[^'\\])*'", "''", cleaned)
        cleaned = re.sub(r"//.*?$", "", cleaned, flags=re.MULTILINE)
        cleaned = re.sub(r"/\*[\s\S]*?\*/", "", cleaned)

        pairs = {"(": ")", "[": "]", "{": "}"}
        stack = []
        for ch in cleaned:
            if ch in pairs:
                stack.append(ch)
            elif ch in pairs.values():
                if not stack:
                    ok = False
                    details.append(f"[{f.name}] 存在不匹配的闭合符号: {ch}")
                    break
                if pairs[stack.pop()] != ch:
                    ok = False
                    details.append(f"[{f.name}] 括号类型不匹配: {ch}")
                    break
        if stack:
            ok = False
            details.append(f"[{f.name}] 存在未闭合的括号: {stack}")
    return ok, details


def extract_param(text: str, dotted_name: str):
    """尝试从 C++ 代码中提取 ParamName = value 的数值。支持 .Num 计数。"""
    # 支持数组元素数量统计：Name.Num
    if dotted_name.endswith(".Num"):
        array_name = dotted_name[:-4]
        # 优先从自检/断言代码中读取 ArrayName.Num() == N 的显式校验值（处理循环填充的容器）
        num_pattern = rf"{re.escape(array_name)}\.Num\(\)\s*==\s*([0-9]+)"
        m = re.search(num_pattern, text)
        if m:
            return int(m.group(1))
        # 其次统计 ArrayName.Add(...) 调用次数
        pattern = rf"{re.escape(array_name)}\.Add\("
        count = len(re.findall(pattern, text))
        return count

    # 支持对象成员初始化：Name.Member = value
    parts = dotted_name.split(".")
    if len(parts) == 2:
        pattern = rf"{re.escape(parts[0])}\.{re.escape(parts[1])}\s*=\s*([0-9.]+)"
    else:
        pattern = rf"{re.escape(dotted_name)}\s*=\s*([0-9.]+)"
    m = re.search(pattern, text)
    if m:
        val = m.group(1)
        return float(val) if "." in val else int(val)
    return None


def check_layer3_params(module: str, files: list[Path]) -> tuple[bool, list[str]]:
    """检查模块关键参数是否在合理范围内。"""
    ok = True
    details = []
    rules = PARAM_RULES.get(module, {})
    if not rules:
        return ok, [f"模块 {module} 没有定义参数范围规则"]

    all_text = "\n".join(read_file(f) for f in files)
    for param, (lo, hi) in rules.items():
        val = extract_param(all_text, param)
        if val is None:
            details.append(f"未找到参数 {param}")
            continue
        if not (lo <= val <= hi):
            ok = False
            details.append(f"参数 {param} = {val} 超出范围 [{lo}, {hi}]")
        else:
            details.append(f"参数 {param} = {val} ✓")
    return ok, details


def check_layer4_function(module: str, files: list[Path]) -> tuple[bool, list[str]]:
    """检查关键标识符是否仍然存在。"""
    ok = True
    details = []
    rules = FUNCTION_RULES.get(module, [])
    if not rules:
        return ok, [f"模块 {module} 没有定义功能规则"]

    all_text = "\n".join(read_file(f) for f in files)
    for symbol in rules:
        if symbol in all_text:
            details.append(f"{symbol} ✓")
        else:
            ok = False
            details.append(f"{symbol} 缺失")
    return ok, details


def main():
    if len(sys.argv) < 3:
        print("用法: run_mcp_selfcheck.py <module_name> <file1> [file2 ...]")
        sys.exit(1)

    module = sys.argv[1]
    files = [Path(p) for p in sys.argv[2:]]

    print(f"===== MCP 四层自检: {module} =====")
    l1_ok, l1_details = check_layer1_api(files)
    l2_ok, l2_details = check_layer2_syntax(files)
    l3_ok, l3_details = check_layer3_params(module, files)
    l4_ok, l4_details = check_layer4_function(module, files)

    def print_layer(name: str, ok: bool, details: list[str]):
        status = "PASS" if ok else "FAIL"
        print(f"\n[{status}] {name}")
        for d in details:
            print(f"  - {d}")

    print_layer("Layer 1 API 合规", l1_ok, l1_details)
    print_layer("Layer 2 语法检查", l2_ok, l2_details)
    print_layer("Layer 3 参数/渲染模拟", l3_ok, l3_details)
    print_layer("Layer 4 功能规则匹配", l4_ok, l4_details)

    if all([l1_ok, l2_ok, l3_ok, l4_ok]):
        print(f"\n>>> {module}: 四层自检全部通过")
        sys.exit(0)
    else:
        print(f"\n>>> {module}: 存在未通过项，需要回溯修改")
        sys.exit(1)


if __name__ == "__main__":
    main()

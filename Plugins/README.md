# GuangzhouOpenWorld — Plugins Manifest

> **零收入默认（2026-07-21）**: 物理用 **Chaos**，音频用 **UE 内置**。  
> 仓库内 `JoltPhysics` / `SoLoud` 为**可选包装源码缓存**，在 `.uproject` 中 **`Enabled: false`**。  
> 完整选型见 [Docs/TECH_STACK_FREE.md](../Docs/TECH_STACK_FREE.md) · [Docs/ENGINE_STRATEGY.md](../Docs/ENGINE_STRATEGY.md)。  
> 下表「意图启用」≠ 已在真机真图验证。

---

## 默认策略

| 类别 | 策略 |
|------|------|
| UE5.8 原生（PCG、Mass*、Niagara*、Water、Landscape、Recast 等） | `.uproject` 中保持启用意图 |
| **JoltPhysics** | **Disabled**（可选；重新启用前需自备可链接二进制） |
| **SoLoud** | **Disabled**（可选；默认 UE Audio） |
| 未随仓库提供的 OSMImporter / PCGAdvanced 等 | 文档曾提及；**以 `.uproject` 实际条目为准**（当前未单独打包进本仓库 Plugins/） |

---

## Engine Bundled / Project-Enabled（意图）

### Graphics & Rendering

| Plugin | Version | Purpose | Notes |
|--------|---------|---------|-------|
| Niagara | UE5.8 | GPU particle VFX | 引擎自带意图 |
| NiagaraFluids | UE5.8 | Fluid simulation | 引擎自带意图 |
| ProceduralMeshComponent | UE5.8 | Runtime mesh generation | 已在 `.uproject` 启用 |
| Substrate | UE5.8 | Material system | 配置意图；随引擎 |

### Physics

| Plugin / System | Version | Purpose | Notes |
|-----------------|---------|---------|-------|
| **Chaos**（引擎原生） | UE5.8 | 默认物理 | **零收入优先**；`PhysicsEngineInterface=Chaos` |
| JoltPhysics（本仓库插件） | v6.0.1 目标 | 可选第三方物理 | **Enabled: false**；源码保留 |

### World & Streaming

| Plugin | Version | Purpose | Notes |
|--------|---------|---------|-------|
| PCG | UE5.8 | Procedural content | `.uproject` 启用意图 |
| WorldPartitionHLOD | UE5.8 | HLOD | `.uproject` 启用意图 |
| Landmass / Landscape / Water | UE5.8 | 地形与水体 | `.uproject` 启用意图 |

### AI & Crowd

| Plugin | Version | Purpose | Notes |
|--------|---------|---------|-------|
| MassAI / MassCrowd / MassTraffic / MassGameplay | UE5.8 | 人群与车流 | `.uproject` 启用意图；未实测万人级 |
| RecastNavigation | UE5.8 目标钉 | NavMesh | `.uproject` 启用意图 |

### Audio

| Plugin / System | Version | Purpose | Notes |
|-----------------|---------|---------|-------|
| **UE Audio**（默认） | UE5.8 | 主音频路径 | AudioCapture / Synthesis / SoundFields 等保持启用意图 |
| SoLoud（本仓库插件） | 2026 目标 | 可选第三方音频 | **Enabled: false**；源码保留 |

### Network

| Plugin | Version | Purpose | Notes |
|--------|---------|---------|-------|
| OnlineSubsystemEOS 等 | UE5.8 | 联机 | 项目侧 `USE_REAL_EOS_SDK=0` stub；真 SDK 另接 |

---

## 本仓库 `Plugins/` 目录

| 目录 | `.uproject` | 说明 |
|------|-------------|------|
| `Plugins/JoltPhysics/` | Disabled | 包装 + 自检；无完整上游二进制 |
| `Plugins/SoLoud/` | Disabled | 包装 + 自检；无完整上游二进制 |

重新启用时：改为 `"Enabled": true`，按 `Scripts/ThirdPartyBuild/` 编出库，并恢复 `DefaultEngine.ini` 中对应开关（当前为关闭）。

---

## Disabled（项目策略）

| Plugin | Reason |
|--------|--------|
| **JoltPhysics** | Chaos 优先；降低无二进制时的打开失败风险 |
| **SoLoud** | UE 内置音频优先 |
| OpenGL / Vulkan / DirectX | Mac Metal 目标路径 |
| Hardware Ray Tracing | Apple Silicon 目标路径不依赖 |
| 各 VR / 非 Mac 平台插件 | 非当前目标 |

---

## Build Notes（有 UE5.8 的环境）

1. 克隆仓库后直接用 UE5.8 打开；确认 Jolt/SoLoud 为 Disabled。  
2. 可选：`Scripts/macOS/build-mac.sh`（需本机引擎路径）。  
3. 勿把「插件清单存在」写成「已 Shipping 出包验证」。

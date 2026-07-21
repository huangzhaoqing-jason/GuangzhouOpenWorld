# 实现状态对照表（REALITY STATUS）

> **更新日期**: 2026-07-21  
> **用途**: 分清「仓库里有什么」和「能不能当游戏玩」。  
> **图例**:
> - **代码草稿** — 有 C++/Swift 逻辑或参数，未在真机真图上验证  
> - **Stub** — 明确降级/打日志/空实现，缺真实 SDK 或运行时  
> - **配置声明** — 仅写在 `.ini` / `.uproject` / checklist，不等于可用  
> - **缺失** — Content、第三方二进制或地图资产不存在  

相关文档：[ROADMAP_3A.md](ROADMAP_3A.md) · [README.md](../README.md)

---

## 1. 一句话结论

本仓库是 **广州开放世界的系统脚手架与目标架构**，不是可玩的 3A 游戏。

| 维度 | 现状 |
|------|------|
| 可游玩地图 / 模型 / 材质 | **缺失**（`Content/` 几乎为空） |
| 默认关卡 `GZ_MainWorld` | **缺失**（`Config/DefaultEngine.ini` 已引用） |
| 引擎 / 第三方真实版本 | **目标声明**，本仓库环境未安装 UE，无法编译验证 |
| 联机 / 云存档 EOS | **Stub**（`USE_REAL_EOS_SDK=0`） |
| Jolt / SoLoud 插件 | **包装模块**，无完整上游 SDK 二进制 |
| 旧「4A+ 175/175」清单 | **配置声明 ≠ 实测通过** |

---

## 2. Content 与工程入口

| 项 | 路径 / 引用 | 状态 | 说明 |
|----|-------------|------|------|
| 游戏地图 | `/Game/Maps/GZ_MainWorld` | **缺失** | `DefaultEngine.ini` GameDefaultMap / EditorStartupMap |
| `Content/` | `Content/` | **缺失** | 仅有 `MetalShaderLibraries/` 占位目录 |
| 城市场景资产 | — | **缺失** | 无 Nanite 建筑、道路、植被、车辆网格 |
| 音频资产 | — | **缺失** | 无 WAV/OGG 内容资源 |
| UI 蓝图 / UMG 资产 | — | **缺失** | 登录/菜单多为 C++ Widget 类，无配套 Content |
| `.uproject` | `GuangzhouOpenWorld.uproject` | **配置声明** | `EngineAssociation=5.8` 等为**目标**关联，非「已交付编译」证明 |

---

## 3. 游戏与玩法（`Source/.../Game/`）

| 模块 | 主要文件 | 状态 | 说明 |
|------|----------|------|------|
| GameMode（昼夜/天气/画质档） | `GZGameMode` | **代码草稿** | 大量参数与状态机；缺关卡与渲染资产驱动 |
| 角色 / 相机 | `GZCharacter` | **代码草稿** | 第三人称骨架逻辑；缺动画/网格 |
| 玩家控制（武器/通缉/载具输入） | `GZPlayerController` | **代码草稿** | 未与真实关卡、Mass、物理联调 |
| 存档 | `GZSaveGame` / `GZSaveGameManager` | **代码草稿** | 本地存档路径存在；云端依赖 EOS stub |
| 任务 / 背包 / 设置 | `GZMissionManager` 等 | **代码草稿** | 管理器级 API，无 Content 任务数据 |
| 登录 / 账号 | `GZAccountLoginManager` + EOS | **Stub** | 显式依赖真实 EOS SDK |
| 主菜单 / 选角 Widget | `GZMainMenuWidget` 等 | **代码草稿** | 无验证过的 UMG 资源与流程 |
| 城市事件 / 洪水 / 职业 / 双角色等 | `Game/Gameplay/*` | **代码草稿** | 系统设计稿级代码，未达可玩切片 |
| MCP 自检 | `GZMCPSelfCheck` | **代码草稿** | 规则/语法层自检，不能替代真机游玩验收 |

---

## 4. AI 与导航（`AI/RecastMassAI/`）

| 模块 | 状态 | 说明 |
|------|------|------|
| `GZMassAI`（宣称 12k+ 代理） | **代码草稿** + **配置声明** | 参数与 LOD 逻辑存在；无 NavMesh 资产与实测 |
| `GZNavigationSystem` | **代码草稿** | Recast 封装；缺烘焙导航数据 |
| 分区预算 / NPC 记忆 / 交通冲突 | **代码草稿** | 未与真实城区几何绑定 |

**未实测**: 「12000 智能体同时跑在广州地图上」。

---

## 5. 物理（`Physics/` + `Plugins/JoltPhysics`）

| 模块 | 状态 | 说明 |
|------|------|------|
| 车辆 / 水体 / 破坏 / 风力 | **代码草稿** | 多为算法与参数模拟接口 |
| Jolt 插件 | **Stub / 包装** | 内存池、桥接、自检；无完整 Jolt v6.0.1 运行时二进制 |
| CMake 第三方脚本 | **配置声明** | `Scripts/ThirdPartyBuild/` 需本机实际编出库才算可用 |

**未实测**: 16-DOF 车辆、SPH 水面、大规模破坏在 Mac 真机上的表现。

---

## 6. 场景与渲染（`Scene/` + `Shaders/`）

| 模块 | 状态 | 说明 |
|------|------|------|
| `GZWorldPartition` 包装 | **代码草稿** | 网格/预加载参数；无 WP 地图 |
| `GZPCGGenerator` 八区模板 | **代码草稿** | 区划 Bounds/种子等**数据模板**，不是流送城市几何 |
| Metal 着色器（LiquidGlass 等） | **代码草稿** | `.usf` 源文件存在；未在 UE 编辑器中编译验证 |
| Gaussian / Spark 桥 | **Stub / 草稿** | 跨端桥接；非 macOS 路径多为降级 |
| Lumen / Nanite / VSM / TSR CVars | **配置声明** | `DefaultEngine.ini` 写满；无场景则无法验收画质 |

---

## 7. 音频与联机

| 模块 | 状态 | 说明 |
|------|------|------|
| `GZAudioManager` + SoLoud 插件 | **Stub / 包装** | 通道组与桥接声明；无 SoLoud 2026 完整库与音频资产 |
| `GZNetworkManager` | **代码草稿** | Session/气泡逻辑偏声明 |
| `GZEOSAuthInterface` | **Stub** | 登录/云存档/加解密均打日志降级 |

---

## 8. Mac 壳与适配层

| 模块 | 状态 | 说明 |
|------|------|------|
| SwiftUI `MacApp`（Open广州 / Liquid Glass） | **代码草稿** | UI 壳与审计文档存在；未与可玩 UE 关卡闭环 |
| `Adapter/*`（MetalFX / EOS / Jolt / CVar） | **代码草稿** | 跨平台宏与降级路径；真实 SDK 仍待接入 |
| `Bridge/*`（SwiftUI / 共享纹理） | **代码草稿** | 需本机 Mac + UE 联调 |

---

## 9. 文档与历史叙事（阅读方式）

| 文档 | 应如何读 |
|------|----------|
| [4A_STANDARDS_CHECKLIST.md](4A_STANDARDS_CHECKLIST.md) | **目标标准 / 配置核对表**，不是「3A 已完成」证明 |
| [PROJECT_ARCHITECTURE.md](PROJECT_ARCHITECTURE.md) | **目标架构说明**，实现进度以本文为准 |
| [5Y_OPTIMIZATION_SUMMARY.md](5Y_OPTIMIZATION_SUMMARY.md) | **历史优化叙事**，不可作为可玩完成度证据 |
| `Docs/OptimizationLogs/*` | 自动/过程日志，同样不证明 Content 或真机验收 |

---

## 10. 与里程碑的映射

| 现状档 | 大致卡在 |
|--------|----------|
| 文档诚实化 | **M0**（本文档所属） |
| 无 UE / 无 Content | **未进入 M1 / M2** |
| 天气/角色/车辆草稿 | 最早服务于 **M3**，当前未达标 |
| 八区 PCG 模板 / Mass 12k | **M4+** 才有意义，当前不可验收 |

下一步请看：[ROADMAP_3A.md](ROADMAP_3A.md)。

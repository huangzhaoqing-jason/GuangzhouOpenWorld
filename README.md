# 广州开放世界（Guangzhou Open World）

**目标**：以广州市为背景的开放世界游戏（自由探索方向对标 GTA / 赛博朋克），质量目标为可验证的 **3A（AAA）**。  
**平台意向**：Apple Silicon Mac（macOS 14+）。

---

## 先读这三句（现状）

1. **现在还不能当游戏玩。** `Content/` 几乎为空，没有可加载的城市地图与美术资源。  
2. **仓库里主要是系统脚手架**：C++ / 配置 / SwiftUI 壳、接口声明与部分逻辑草稿；EOS、Jolt、SoLoud 等存在 **stub 或包装层**。  
3. **旧文档里的「4A+ / 175 项全绿」不等于已完成。** 那些多半是配置声明，不是真机真图验收。

详细对照：[Docs/REALITY_STATUS.md](Docs/REALITY_STATUS.md)  
怎么做到 3A：[Docs/ROADMAP_3A.md](Docs/ROADMAP_3A.md)（里程碑 M0→M5）

---

## 项目定位

| 项 | 说明 |
|----|------|
| 场景目标 | 还原广州核心城区；首个可玩切片定为 **天河 / 珠江新城 CBD 一小块**（见路线图 M3） |
| 质量目标 | **3A**：以游玩录像、帧率与验收清单为准，不以 checklist 勾选为准 |
| 技术栈 | 以 **本机可安装的正式版 Unreal Engine 5** 为准；仓库中 UE 5.8 / Metal 4.3 / SoLoud 2026 等版本号视为 **目标/历史声明**，开工前须按 Epic 实际发布版本重新锁定 |
| 工作模式 | 学习与研究向工程；资产未获官方授权前不得当作商业发布内容 |

---

## 仓库里有什么 / 没有什么

**有：**

- `Source/GuangzhouOpenWorld/` — GameMode、角色、AI/物理/音频/联机等 C++ 草稿与管理器  
- `Config/` — 渲染、Mass、World Partition 等 CVar 与项目配置  
- `MacApp/` — SwiftUI Liquid Glass 壳（Open广州）  
- `Plugins/JoltPhysics`、`Plugins/SoLoud` — 插件包装与自检入口  
- `Shaders/MetalShaders/` — LiquidGlass 等着色器源文件  
- `Docs/` — 架构、状态表、3A 路线图  

**没有（关键缺口）：**

- 可玩 `.umap` / 城市场景 / 角色与车辆网格  
- 已验证的本机 UE 编译产物  
- 完整 Jolt / SoLoud / EOS 运行时与云存档  

---

## 目录结构

```
GuangzhouOpenWorld/
├── Config/                  # 引擎与项目配置（声明 ≠ 已验收）
├── Content/                 # 美术与关卡（当前几乎为空）
├── Docs/
│   ├── REALITY_STATUS.md    # 实现状态对照（必读）
│   ├── ROADMAP_3A.md        # 3A 里程碑 M0–M5（必读）
│   ├── PROJECT_ARCHITECTURE.md
│   ├── 4A_STANDARDS_CHECKLIST.md  # 目标标准/配置核对，非完成证明
│   └── OptimizationLogs/    # 历史过程日志，非完成证明
├── MacApp/                  # SwiftUI 壳
├── Plugins/                 # Jolt / SoLoud 包装
├── Scripts/                 # macOS / 第三方构建脚本
├── Shaders/MetalShaders/
└── Source/GuangzhouOpenWorld/
```

---

## 里程碑（摘要）

| 阶段 | 含义 | 当前 |
|------|------|------|
| M0 | 文档诚实化 | 本 README 所属 |
| M1 | 本机正式版 UE 可打开工程 | 需自备引擎 |
| M2 | 空白关卡第三人称可玩 | 未开始 |
| M3 | CBD 垂直切片（原型门槛） | 未开始 |
| M4 | 多城区 + 流送稳定 | 未开始 |
| M5 | 玩法/音频/性能达 3A 验收清单 | 未开始 |

完整 DoD 与「禁止宣称」条款见 [Docs/ROADMAP_3A.md](Docs/ROADMAP_3A.md)。

---

## 本地构建指引（macOS，需自备引擎）

### 前置条件

- Apple Silicon Mac（M1 / M2 / M3 等）  
- macOS 14+  
- Xcode（含命令行工具）  
- **已安装的正式版 Unreal Engine 5**（版本以 Epic Launcher / 源码编译为准，并写回 `.uproject` 的 `EngineAssociation`）  
- CMake 3.28+（若启用第三方物理/音频库）

### 建议步骤

```bash
git clone <本仓库 URL>
cd guangzhouopenworld

# 1. 将 GuangzhouOpenWorld.uproject 中 EngineAssociation 改为你本机 UE 版本

# 2. 第三方库：能编则编；编不过就先在 .uproject 里禁用 JoltPhysics / SoLoud
# ./Scripts/ThirdPartyBuild/...   # 以脚本实际路径为准

# 3. 生成工程并编译（路径换成你的引擎安装目录）
# GenerateProjectFiles → Xcode/UBT 编译 → UnrealEditor 打开本工程
```

打包、Metal 着色器脚本等见 `Scripts/macOS/`。在 **M1 未完成** 前，勿把「脚本存在」写成「已成功出包」。

---

## 适配与降级（现状说明）

- `Adapter/GZEOSAdapter.h`：`USE_REAL_EOS_SDK=0` 时走本地存档降级；EOS 认证接口为 **stub**。  
- `Adapter/RHICrossPlatform.h` 等：跨平台宏映射；真实 MetalFX / 超分需本机验证。  
- 插件与 Mass / World Partition：配置可写满，**无 Content 则无法验收**。

---

## 文档阅读顺序

1. [Docs/REALITY_STATUS.md](Docs/REALITY_STATUS.md) — 各模块真实状态  
2. [Docs/ROADMAP_3A.md](Docs/ROADMAP_3A.md) — 下一步做什么  
3. [Docs/PROJECT_ARCHITECTURE.md](Docs/PROJECT_ARCHITECTURE.md) — 目标架构（实现进度以状态表为准）  
4. [Docs/4A_STANDARDS_CHECKLIST.md](Docs/4A_STANDARDS_CHECKLIST.md) — 仅作目标/配置参考  

---

## 声明

本项目为学习与研究性质的游戏工程实践。城市景观、品牌、车辆等素材在获得合法授权前，仅可用于技术验证，不代表任何官方授权或已完成的商业级 3A 产品。

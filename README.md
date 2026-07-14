# 广州开放世界（Guangzhou Open World）

**4A+ 级广州城市开放世界游戏项目 | Apple Silicon Mac 原生适配版**

本项目以广州市为原型，打造高密度、可自由探索的现代都市开放世界。项目基于 Unreal Engine 5.8 的目标架构进行设计，深度绑定 Metal 渲染后端、Chaos / Jolt 物理、Mass AI、光线追踪、DLSS/FSR 超分适配、空间音频与 EOS 联机存档体系，面向 M1/M2/M3 系列 Apple Silicon Mac 进行原生优化。

---

## 项目定位

- **场景规模**：完整还原广州核心城区，支持昼夜循环、动态天气、道路积水干湿变化、交通 NPC 24 小时生命周期。
- **目标平台**：Apple Silicon Mac（M1 / M2 / M3），兼容 macOS 14+。
- **画质基准**：光影物理真实性、区块无缝程度、玻璃光学精度、天气层次表现、AI 行为丰富度等 10 大维度对标《荒野大镖客2 终极版 2026》《GTA5 增强版 1.72》。
- **工作模式**：7×24 全自动闭环工程，所有代码、配置与优化日志统一存放于本 GitHub 私有仓库，不写入本地 Mac 文件。

---

## 核心技术栈

| 模块 | 设定版本 / 技术 |
|------|----------------|
| 游戏引擎 | Unreal Engine 5.8（2026.6.23 最终稳定版目标） |
| 渲染后端 | Metal 4.3 / MetalFX Upscaling / MSL 3.1 |
| 超分与帧生成 | DLSS 4.5 / FSR 4（Mac 上映射到 MetalFX 等效实现） |
| 物理引擎 | Chaos + Jolt Physics v6.0.1（Apple Silicon NEON + UMA 优化） |
| 音频引擎 | SoLoud 2026 + Apple Core Audio 空间音频 |
| AI 与交通 | UE5.8 Mass Entity + MassTraffic，单场景 12,000+ 智能代理 |
| 联机与存档 | EOS SDK 1.19.0.7（当前启用 `USE_REAL_EOS_SDK=0` 本地降级存档） |
| 异步 IO | DirectStorage（Windows）/ `FStreamableManager` + `AsyncLoading`（Mac） |
| 质量保障 | MCP 四层自检：API 合规、编译语法、云端渲染模拟、功能规则匹配 |

> **技术栈说明**：仓库代码按上述目标版本进行接口声明、宏定义与适配层映射。当前公开渠道中，UE 5.8、Metal 4.3、DLSS 4.5、FSR 4、EOS SDK 1.19.0.7、SoLoud 2026、Jolt Physics v6.0.1 尚未正式发布；真实编译前需以官方实际发布版本为准，或通过适配头文件回退到已发布的等效接口。

---

## 关键特性

- **光影系统**：Lumen 全局光照、光追 RTGI / RT 阴影 / RTAO、路径追踪反射、分距离采样（0–50 m 32 spp / 50–200 m 16 spp / 200 m+ 8 spp）。
- **玻璃光学**：双层 IOR、Fresnel 反射曲线、边缘厚度渐变、0.5 px 帧混合，消除折射断层与蓝色滤镜。
- **天气与路面**：晴天 / 多云 / 雨 / 暴雨 / 雾霾五层天气状态机，路面干湿三段式模型（Dry / Transitioning / Wet），积水动态反射。
- **物理交互**：风力驱动的植被、广告牌、树木摆动；车辆悬挂、刹车距离、离心力与路面积水阻力；0–200 m 全精度 AI 物理、200–800 m 低频更新、800 m+ 仅位置同步。
- **自适应性能**：M1/M2/M3 三档芯片配置，TSR 八档分辨率缩放，低配流畅、高配画质拉满。

---

## 目录结构

```
GuangzhouOpenWorld/
├── Config/                  # 引擎与项目配置
├── Content/                 # 美术资源（由 git-lfs 管理）
├── Docs/
│   ├── 4A_STANDARDS_CHECKLIST.md
│   └── OptimizationLogs/    # 自动生成的优化日志
├── Plugins/
│   ├── JoltPhysics/         # Jolt Physics v6.0.1 插件
│   └── SoLoud/              # SoLoud 2026 音频插件
├── Scripts/
│   └── ThirdPartyBuild/     # Jolt / SoLoud macOS 编译脚本
└── Source/
    └── GuangzhouOpenWorld/
        ├── Adapter/         # 跨平台适配头文件
        │   ├── RHICrossPlatform.h
        │   ├── JoltVersionAdapter.h
        │   ├── GZEOSAdapter.h
        │   └── GZCVarAdapter.h
        ├── Game/            # GameMode、GameInstance、MCP 自检等
        ├── Physics/         # 风力、车辆、碰撞相关组件
        ├── Rendering/       # Lumen、TSR、玻璃、天气渲染逻辑
        ├── AI/              # Mass AI、交通、NPC 生命周期
        └── UI/              # iOS27 液态玻璃风格 UI 与主菜单
```

---

## 本地构建指引（macOS）

### 前置条件

- Apple Silicon Mac（M1/M2/M3）
- macOS 14 Sonoma 或更高
- Xcode 16.x（命令行工具已安装）
- Unreal Engine 5.8（官方正式发布后）
- CMake 3.28+

### 编译步骤

```bash
# 1. 克隆仓库
git clone https://github.com/huangzhaoqing-jason/guangzhouopenworld.git
cd guangzhouopenworld

# 2. 编译第三方库
chmod +x Scripts/ThirdPartyBuild/build-jolt-macos.sh
chmod +x Scripts/ThirdPartyBuild/build-soloud-macos.sh
./Scripts/ThirdPartyBuild/build-jolt-macos.sh
./Scripts/ThirdPartyBuild/build-soloud-macos.sh

# 3. 生成 Xcode 工程
/Path/To/UE_5.8/Engine/Build/BatchFiles/Mac/GenerateProjectFiles.sh \
  -project=$(pwd)/GuangzhouOpenWorld.uproject \
  -game -engine

# 4. 打开并编译
open GuangzhouOpenWorld.xcworkspace
# 在 Xcode 中选择 Scheme: GuangzhouOpenWorldEditor (Mac)，Build (⌘+B)

# 5. 启动编辑器
/Path/To/UE_5.8/Engine/Binaries/Mac/UnrealEditor.app/Contents/MacOS/UnrealEditor \
  $(pwd)/GuangzhouOpenWorld.uproject
```

### 打包发布

```bash
/Path/To/UE_5.8/Engine/Build/BatchFiles/RunUAT.sh BuildCookRun \
  -project=$(pwd)/GuangzhouOpenWorld.uproject \
  -noP4 -platform=Mac \
  -clientconfig=Shipping -serverconfig=Shipping \
  -cook -allmaps -stage -pak -archive \
  -archivedirectory=$(pwd)/Packaged
```

---

## 跨平台适配说明

项目通过新增适配头文件实现 Mac / Windows 兼容性，**不修改业务代码主体**。

- `RHICrossPlatform.h`：Mac 上将 DLSS/FSR 映射到 MetalFX，DirectStorage 映射到 `FStreamableManager`；Windows 保留原版接口。
- `JoltVersionAdapter.h`：保留 `JOLT_VERSION_601` 宏与全部物理参数，仅 Mac 屏蔽 v6 专属 GPU 接口。
- `GZEOSAdapter.h`：`USE_REAL_EOS_SDK=0` 时启用本地存档降级，接口声明完全不变，开启宏即可恢复 EOS 云端逻辑。
- `GZCVarAdapter.h`：GameMode `BeginPlay()` 统一注册全部自定义控制台变量，带空指针保护。

---

## MCP 四层自检

每个子任务完成后执行：

1. **API 合规自检**：校验 UE5.8 / Metal 原生接口，剔除废弃接口。
2. **编译语法自检**：clangd / UBT 语法、依赖、反射宏检查。
3. **云端渲染模拟自检**：检测光影断层、模型穿模、物理异常、UI 瑕疵；不可用时自动跳过。
4. **功能规则匹配自检**：校验所有逻辑是否符合项目标准。

自检代码位于 `Source/GuangzhouOpenWorld/Game/MCPSelfCheck/`。

---

## 贡献与迭代

- 本项目采用全自动闭环开发模式，优化日志自动写入 `Docs/OptimizationLogs/`。
- 历史版本通过 Git tag 管理，支持云端回滚。
-  issues、 milestones 与 pull requests 均通过本仓库管理。

---

## 声明

本项目为学习与研究性质的游戏工程实践，所有城市景观、品牌标识、车辆型号等素材仅用于技术验证，不代表任何官方授权。实际发布前请替换为合法授权资源。

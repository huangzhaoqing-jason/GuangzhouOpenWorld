# 广州开放世界（Guangzhou Open World）

**目标**：以广州市为背景的开放世界游戏（自由探索方向对标 GTA / 赛博朋克），质量目标为可验证的 **3A（AAA）**。  
**引擎钉**：Unreal Engine **5.8**（将来 **UE6 整仓替换**）。  
**平台意向**：Apple Silicon Mac（有引擎的环境）；维护者也可 **仅用 GitHub 作代码缓存**。

---

## 先读这几句（现状）

1. **现在还不能当游戏玩。** `Content/` 几乎为空，没有可加载的城市地图与美术资源。  
2. **仓库是系统脚手架 + 文档缓存**：C++ / 配置 / SwiftUI 壳；EOS 等为 stub；**Jolt / SoLoud 默认关闭**，物理/音频走 UE 原生（Chaos + 内置 Audio）。  
3. **旧「4A+ 全绿」≠ 已完成。** 配置声明不是真机真图验收。  
4. **本机装不了引擎也没关系**：继续把代码和文档推到 GitHub；游玩验收留给有 **UE5.8** 的环境（路线图 **M1b** 起）。

| 必读 | 链接 |
|------|------|
| 实现状态 | [Docs/REALITY_STATUS.md](Docs/REALITY_STATUS.md) |
| 3A 里程碑 | [Docs/ROADMAP_3A.md](Docs/ROADMAP_3A.md) |
| 免费技术栈 | [Docs/TECH_STACK_FREE.md](Docs/TECH_STACK_FREE.md) |
| 引擎策略（5.8→UE6） | [Docs/ENGINE_STRATEGY.md](Docs/ENGINE_STRATEGY.md) |

---

## 项目定位

| 项 | 说明 |
|----|------|
| 场景目标 | 广州核心城区；首个可玩切片：**天河 / 珠江新城 CBD 一小块**（M3） |
| 质量目标 | **3A**：游玩录像与验收清单，不以 checklist 勾选为准 |
| 技术栈 | **UE5.8 原生免费模块优先**（Nanite / Lumen / Substrate / WP / PCG / Chaos / Mass 等）+ Blender / GDAL+PDAL / Audacity / LMMS |
| 授权 | 零收入阶段按 Epic 与各工具**官方条款**；Megascans / MetaHuman **不入库**，有引擎后再取 |
| 工作模式 | GitHub 为完整代码缓存；学习研究向；未授权资产不得当商业成品 |

---

## 仓库里有什么 / 没有什么

**有：**

- `Source/GuangzhouOpenWorld/` — 玩法与系统 C++ 草稿  
- `Config/` — 渲染 / Mass / World Partition 等配置声明  
- `MacApp/` — SwiftUI 壳  
- `Plugins/JoltPhysics`、`Plugins/SoLoud` — **可选包装源码（默认 Disabled）**  
- `Shaders/MetalShaders/`、`Docs/`（含免费栈与引擎策略）  

**没有：**

- 可玩地图与城市场景 Content  
- 本环境已验证的 UE5.8 编译产物  
- 默认启用的 Jolt / SoLoud 运行时、真实 EOS 云存档  

---

## 目录结构

```
GuangzhouOpenWorld/
├── Config/
├── Content/                 # 当前几乎为空
├── Docs/
│   ├── REALITY_STATUS.md
│   ├── ROADMAP_3A.md
│   ├── TECH_STACK_FREE.md   # 零收入免费技术栈
│   ├── ENGINE_STRATEGY.md   # UE5.8 钉与 UE6 整换
│   └── ...
├── MacApp/
├── Plugins/                 # Jolt/SoLoud 默认关闭
├── Scripts/
├── Shaders/MetalShaders/
└── Source/GuangzhouOpenWorld/
```

---

## 里程碑（摘要）

| 阶段 | 含义 | 当前 |
|------|------|------|
| M0 | 文档诚实化 | **已完成** |
| M1 | 仓库侧：5.8 钉 + 免费栈 + 插件策略 | **已完成** |
| M1b | 有 UE5.8 的环境打开工程 | 未开始 |
| M2 | 空白关卡可玩 | 未开始 |
| M3 | CBD 垂直切片 | 未开始 |
| M4–M5 | 扩城与 3A 打磨 | 未开始 |

---

## 有 UE5.8 时怎么开工程

```bash
git clone <本仓库 URL>
cd GuangzhouOpenWorld
# EngineAssociation 已钉 5.8；勿改成「已交付」叙事
# JoltPhysics / SoLoud 应为 Disabled；物理用 Chaos，音频用 UE 内置
# 使用本机安装的 UE5.8 生成工程并打开 UnrealEditor
```

Megascans / MetaHuman / Blender 资产按 [TECH_STACK_FREE.md](Docs/TECH_STACK_FREE.md) 在引擎侧获取，**不要把大型商业缓存硬塞进 Git**。

无引擎时：只改文档与源码并 `git push`，完成度看 M0/M1，不看假编译。

---

## 适配说明

- EOS：`USE_REAL_EOS_SDK=0` 本地降级；认证接口为 stub。  
- 物理：**Chaos 优先**；Jolt 包装默认关闭。  
- 音频：**UE 内置优先**；SoLoud 默认关闭。  

---

## 文档阅读顺序

1. [Docs/REALITY_STATUS.md](Docs/REALITY_STATUS.md)  
2. [Docs/TECH_STACK_FREE.md](Docs/TECH_STACK_FREE.md)  
3. [Docs/ENGINE_STRATEGY.md](Docs/ENGINE_STRATEGY.md)  
4. [Docs/ROADMAP_3A.md](Docs/ROADMAP_3A.md)  
5. [Docs/PROJECT_ARCHITECTURE.md](Docs/PROJECT_ARCHITECTURE.md)（目标架构；进度以状态表为准）  

---

## 声明

本项目为学习与研究性质的游戏工程实践。城市景观、品牌、车辆等素材在获得合法授权前，仅可用于技术验证，不代表官方授权或已完成的商业级 3A 产品。引擎分成与商用条件以 **Epic 官方 EULA** 为准。

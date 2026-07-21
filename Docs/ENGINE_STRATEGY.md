# 引擎策略（UE5.8 → UE6 整仓替换）

> **当前钉**: Unreal Engine **5.8**（`GuangzhouOpenWorld.uproject` → `EngineAssociation`）  
> **未来**: Unreal Engine **6** 正式发布且可用后，**一次性整仓迁移替换**（不长期双引擎并行）  
> **开发模式**: 维护者本机可能无法安装/运行引擎；**GitHub 仓库 = 唯一免费完整代码与文档缓存**

相关：[TECH_STACK_FREE.md](TECH_STACK_FREE.md) · [ROADMAP_3A.md](ROADMAP_3A.md) · [REALITY_STATUS.md](REALITY_STATUS.md)

---

## 1. 为什么钉 5.8

- 项目以 **UE5.8 原生免费模块**（Nanite、Lumen、Substrate、VSM、TSR、World Partition、PCG、Chaos、Mass 等）为选型基线，见技术栈文档。  
- `EngineAssociation = "5.8"` 表示 **目标引擎钉**，不是「本环境已编译通过」的证明。  
- 零收入阶段不引入付费引擎替代品；辅助工具用 Blender / GDAL+PDAL / Audacity / LMMS 等免费链。

---

## 2. 远程 / 无本机引擎工作方式

| 角色 | 做什么 |
|------|--------|
| 无引擎的维护者 | 只改 Git 中的 Source / Config / Docs / Scripts；推送到 GitHub |
| 有 UE5.8 的机器或云环境 | 克隆仓库 → 执行路线图 **M1b** 起的编译与游玩验收 |
| 本仓库 / CI | **不假装**已用 UE5.8 编辑器验证通过 |

禁止把「文档写了 5.8」写成「游戏已可在 5.8 上游玩」。

---

## 3. 第三方插件策略（零收入）

| 插件 | 策略 |
|------|------|
| PCG / Mass* / WorldPartitionHLOD / Niagara* / Water / Landscape / Landmass / RecastNavigation 等 | **保持启用意图**（引擎自带或随引擎） |
| **JoltPhysics** | **默认 `Enabled: false`**；物理走 Chaos |
| **SoLoud** | **默认 `Enabled: false`**；音频走 UE 内置 |
| EOS 真 SDK | 继续 `USE_REAL_EOS_SDK=0` 直至需要且条款允许 |

插件目录可保留源码作缓存；禁用不等于删除。

---

## 4. UE6 整仓替换清单（发布后执行）

不做双轨长期维护。迁移时按序：

1. **备份分支**：从当前 `master` / 工作分支打 `pre-ue6` 标签。  
2. **改钉**: `EngineAssociation` → UE6 实际版本号；更新 README / TECH_STACK / 本文件标题中的「5.8」。  
3. **API / 插件**: 对照 Epic 升级说明，修复废弃 API；检查 Mass、PCG、Substrate、Chaos Vehicles 等破坏性变更。  
4. **弃留第三方**: 重新评估 Jolt/SoLoud；默认仍 Chaos + UE Audio，除非有实测必要。  
5. **Content 重烤**: 材质（Substrate）、Nanite、World Partition、NavMesh、HLOD 按 UE6 工具链重建/重保存。  
6. **Mac / Metal**: 重验 Apple Silicon 渲染与打包脚本（`Scripts/macOS/`）。  
7. **验收**: 至少重跑路线图 M1b→M3 的 DoD；再谈 M4/M5。  
8. **删除**文档中所有「仍以 5.8 为唯一运行时」的过期表述。

UE6 未发布前：只维护本清单，**不实现** UE6 代码路径。

---

## 5. 与里程碑的关系

| 里程碑 | 与引擎的关系 |
|--------|----------------|
| M0 | 文档诚实化（已完成） |
| **M1** | **仓库侧**就绪：5.8 钉、免费栈文档、插件禁用策略（**不要求**维护者本机开编辑器） |
| **M1b** | **有 UE5.8 的环境**：编辑器打开无致命插件错误 |
| M2–M5 | 必须在真实 UE 环境验收 |

详见 [ROADMAP_3A.md](ROADMAP_3A.md)。

---

## 6. 允许 / 禁止的表述

| 允许 | 禁止 |
|------|------|
| 「目标引擎 UE5.8；源码与文档缓存在 GitHub」 | 「UE5.8 已全部交付并可玩广州」 |
| 「UE6 发布后整仓替换」 | 「已完成 UE6 迁移」且未做 |
| 「Chaos 优先；Jolt/SoLoud 默认关闭」 | 「Jolt 16-DOF / SoLoud 2026 已在真机达标」未实测 |
| 「Megascans/MetaHuman 按 Epic 条款另行获取」 | 把未授权资产提交进 Git 并宣称可商用 |

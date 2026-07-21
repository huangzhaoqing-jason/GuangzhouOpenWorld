# 交接清单（你上课期间 agent 推进结果）

> 生成时间意图：2026-07-21 云端 babysit  
> 目的：你回来后一起解决下列**卡点**；无引擎能做的已尽量推进。

---

## 已完成（仓库侧）

| 项 | 位置 / 说明 |
|----|-------------|
| M0 诚实文档 | PR #1 / #2 |
| M1 UE5.8 免费栈 + Chaos 优先 | `Docs/TECH_STACK_*` / `ENGINE_STRATEGY`；Jolt/SoLoud 默认关 |
| 剧情 + 广州数据 + Mac 任务 Mock | PR #2：`STORY_BIBLE`、`missions_act1.json`、区划表、MacApp |
| **本分支 babysit 追加** | 见下 |

### 本分支追加（`cursor/babysit-advance-8a86`）

- C++：`FGZMissionJsonLoader` + `UGZMissionManager::LoadAct1MissionsFromJson`，`GZGameInstance::Init` 自动加载  
- `Content/Data/missions_act1.json`（运行时路径）+ `Content/Maps/README.md`  
- `Docs/Data/district_id_map.json`  
- `Scripts/validate_mission_data.py`、`Scripts/sync_missions_json.sh`  
- 默认地图配置改为指向 **`GZ_BoxWorld`**（文件仍须你在 UE 里创建）  
- PR #1 / #2 CI 全绿、可合并；#2 已含 #1；均已标 ready for review（评论权限不足未能留言）

---

## 需要你回来一起解决的问题

### P0 — 阻塞「真正可玩」

1. **本机 / 云没有 UE5.8**  
   - 你电脑太旧、不能下引擎。  
   - **选项**：租用/借用一台能装 UE5.8 的 Mac 或云主机；或等有引擎的协作者执行 `Docs/M2_PREP_CHECKLIST.md`。  
   - 没有这一步，**M1b / M2 / M3 无法验收**。

2. **`GZ_BoxWorld.umap` 不存在**  
   - 配置已指向它；编辑器首次打开会缺图。  
   - **你要做**：在 UE 里新建空关卡并保存为该名，或临时改回引擎自带 Entry 图。

3. **两个 Draft→Ready 的 PR 如何合并**  
   - 建议：**只合并 #2**，然后关闭 #1（#2 已包含 #1 全部提交）。  
   - 再合并本 babysit 分支 PR（若已开）。  
   - Agent **无权限** `gh pr comment` / 可能无 merge 权限，需你在 GitHub 点 Merge。

### P1 — 设计 / 授权

4. **地标写实模型版权**  
   - 广州塔、花城广场等不能直接用未授权扫描件商用。  
   - 需决定：低模示意 / 购买授权 / 仅用通用 Megascans 街区。

5. **Epic EULA / 分成**  
   - 文档写「以官网为准」；商业化前你要自己核对当前 Epic 条款（零收入阶段通常可开发，发布有门槛）。

6. **剧情语气是否定稿**  
   - 势力名、双身份、「硬盘雨」等可改；改完跑 `Scripts/sync_missions_json.sh` + `validate_mission_data.py`。

### P2 — 工程债（有 UE 后再做）

7. **`EGZCityDistrict` vs PCG 八区 vs `district_id`** 三套命名  
   - 映射表已有；C++ 枚举尚未改成单一权威（避免无 UE 盲改爆编译）。  

8. **OSM / GDAL 真数据**  
   - 脚本指针有了；需你本机下载 OSM（勿提交大 pbf）裁剪珠江新城。  

9. **MacApp 真机 macOS 27**  
   - UI 为设计稿 + Linux stub；Liquid Glass API 需真机验证。  

10. **UE CI job 名存实亡**  
    - Workflow 里 UE5 步骤在 runner 无引擎时直接 skip；绿勾≠编过 UE。  

---

## 建议你回来后的 15 分钟操作

1. GitHub：合并 **PR #2** → 关闭 **PR #1** → 审查并合并 babysit PR。  
2. 若有任何能跑 UE5.8 的环境：打开工程 → 按 `M2_PREP_CHECKLIST` 建 `GZ_BoxWorld` → 走一步第三人称。  
3. 本地：`python3 Scripts/validate_mission_data.py`（应打印 OK）。  
4. 把上面 P0/P1 里你选的方案（引擎来源、合并策略、地标资产策略）告诉 agent，继续下一轮。

---

## 诚实底线（未变）

- 仓库**仍不可玩**广州开放世界。  
- 没有 Content 城市场景，没有引擎编译证明。  
- 当前进度 = **文档 + 数据 + Mac Mock UI + 任务 JSON 加载代码草稿**。

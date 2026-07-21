# 交接清单（HANDOFF）

> 更新：ABC 三线并进后（PR 合并 + M2 手册 + 幕 2 / OSM / Mac）

---

## A — PR 状态（已处理）

| PR | 结果 |
|----|------|
| [#3](https://github.com/huangzhaoqing-jason/GuangzhouOpenWorld/pull/3) | **已合并进 master** |
| #1 / #2 | 亦为 merged（内容已在 master；勿再重复开合） |

本地请：`git checkout master && git pull`。

---

## 仓库里已有（仍不可玩）

- M0/M1 文档、UE5.8 免费栈、Chaos 优先  
- 剧情幕 1 + **幕 2 节拍与 `missions_act2.json`**  
- 广州区划表、`district_id_map.json`  
- Mac 任务日志 / 通缉 Mock（含 +/- 星）  
- 任务 JSON C++ 加载（**幕 1** 自动）  
- **[M2_RUNBOOK.md](../M2_RUNBOOK.md)** — 有引擎时逐步操作  
- `Scripts/osm_crop_zhujiang_example.sh` — 自备 pbf 后裁剪示例（不入库大文件）  
- `python3 Scripts/validate_mission_data.py` — 校验 act1+act2  

---

## 仍须你解决（P0）

1. **弄到 UE5.8 环境**（旧电脑装不了）→ 打开工程跟 **M2_RUNBOOK** 建 `GZ_BoxWorld`。  
2. **地标资产版权**策略（示意 / Megascans 通用 / 授权）。  
3. **幕 2 C++ 自动加载**（可选下一迭代；现仅文档+JSON）。  

没有引擎时，可继续改剧情/JSON/Mac UI，但到不了「可玩」。

---

## 你回来 15 分钟

1. `git pull` master  
2. `python3 Scripts/validate_mission_data.py`  
3. 有 UE：打开 [M2_RUNBOOK.md](../M2_RUNBOOK.md) 第 1–4 步  
4. 无 UE：告诉我们引擎来源计划（借机/云），或指定下一幕剧情修改  

---

## 诚实底线

游戏仍不是可玩的广州开放世界；当前是脚手架 + 数据 + Mac Mock + 手册。

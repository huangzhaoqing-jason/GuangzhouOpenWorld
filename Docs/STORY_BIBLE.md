# 剧情圣经（STORY BIBLE）— Open广州

> **气质参考**：自由都市行动（类 GTA 的城市自由度 / 通缉压力）+ 近未来霓虹与企业阴影（类赛博朋克的氛围）。  
> **法律**：本作**原创**故事与势力名；**不是** Rockstar Games / CD Projekt RED 的作品，不含其商标角色、地名或剧情抄袭。  
> **地理第一幕**：天河 · **珠江新城**（见 [Data/GUANGZHOU_DISTRICTS.md](Data/GUANGZHOU_DISTRICTS.md)）。  
> **引擎对接**：任务类型对齐 `EGZMissionType`（`GZMissionManager`）：MainStory / Side / Daily / Racing / Collection / Wanted。

---

## 一句话世界观

近未来的广州仍在扩张：珠江新城的玻璃幕墙昼夜吞吐资本与数据；江对岸的塔影下，灰产、信息掮客与「秩序外包」在雨夜争夺同一条中轴线。玩家在两套身份之间切换，决定这座城记住你是快递员、目击证人，还是通缉名单上的噪点。

---

## 双身份（呼应 `GZDualCharacterSystem` 草稿）

| 线 | 称呼（可改） | 白天气质 | 夜间气质 |
|----|--------------|----------|----------|
| A · 跑腿/探查 | 「顺路」 | 合法订单、送件、踩点 | 加密外卖、掉包、目击任务 |
| B · 街头生存 | 「底噪」 | 载具活、小冲突、躲债 | 霓虹巷战、企业安保、通缉升级 |

两线在**幕 1 末**汇合：同一份泄露的「花城地下管线权限」把双方绑上主线。

---

## 势力（原创名）

| ID | 名称 | 功能 |
|----|------|------|
| `faction_lumen_hold` | 流明控股 | CBD 企业方：工单、封锁、公关式「清理」 |
| `faction_tide_grid` | 潮汐网格 | 灰产物流与水下光纤；提供黑市载具改装 |
| `faction_order_lease` | 秩序租赁 | 类城管/外包安保：通缉星级、临时封路 |
| `faction_signal_broker` | 信号掮客 | 街头信息商：任务板、Codex 线索、双身份中介 |

---

## 三幕大纲

### 幕 1 — 中轴线（珠江新城）*当前种子*

花城广场到江边的开放轴上，「顺路」接到一单异常夜配；「底噪」同时被秩序租赁盯上。玩家学会：移动、小任务、通缉 0–2 星、阅读 Codex。  
**收束**：在江边目击流明控股销毁硬盘，获得进入幕 2 的权限密钥（道具占位）。

### 幕 2 — 旧城与握手楼（越秀 / 荔湾）*种子已写*

| 节拍 | 任务 id | 区划 | 玩法重点 |
|------|---------|------|----------|
| 2.1 密钥入西关 | `ms_21_key_handoff` | `liwan_xiguan` | 骑楼接点、暗语、潮汐网格 |
| 2.2 握手楼上行 | `ms_22_handshake_climb` | `yuexiu_old_town` | 垂直路径、天台取模块 |
| 2.3 黑市交换 | `ms_23_black_market_swap` | `liwan_xiguan` | 要挟交换、双身份掩护 |
| Side | `sd_21_*` / `sd_22_*` | 西关 / 越秀 | 短送、窃听 |
| Wanted | `wd_21_alley_cooldown` | 西关巷弄 | 3 星降温 |

**收束**：拿到 `signal_relay_mod`，潮汐网格开放跨江线索 → 幕 3。  
数据：[Data/missions_act2.json](Data/missions_act2.json)（C++ 暂只自动加载幕 1；幕 2 有引擎后再挂加载入口）。

### 幕 3 — 跨江与港（海珠 / 黄埔意向）

企业与灰产全面撕破；主线在跨江与港口设施收束。通缉可达高星；开放结局支线（投靠 / 曝光 / 消失）。

---

## 与 `GZMissionManager` 的映射

| `EGZMissionType` | 本圣经用法 |
|------------------|------------|
| MainStory | 幕内主线节拍（`ms_*`） |
| Side | 势力委托、街坊事件（`sd_*`） |
| Wanted | 通缉触发的逃脱/消星（`wd_*`） |
| Daily / Racing / Collection | 幕 2+ 再扩；幕 1 JSON 可留空或各 1 条占位 |

### 加载约定（有 UE 后）

1. 幕 1：`Content/Data/missions_act1.json`（`GZGameInstance` 已调用 `LoadAct1MissionsFromJson`）。  
2. 幕 2：`Docs/Data/missions_act2.json` — 同步到 Content 后增加 `LoadAct2…`（待做）。  
3. Mac 壳 Mock 可读 act1（及后续合并目录）。  
4. 改 JSON 后跑：`python3 Scripts/validate_mission_data.py` 与 `Scripts/sync_missions_json.sh`。

---

## 通缉阶梯（玩法气质，非抄袭星级 UI）

| 等级 | 表现意图 | 任务类型 |
|------|----------|----------|
| 0 | 无压力 | — |
| 1–2 | 巡逻靠近、鸣笛 | Wanted / Side |
| 3+ | 封路、无人机探照（霓虹夜） | Wanted + MainStory 干扰 |

HUD Mock：见 MacApp `LiquidGlassHUD` 通缉条。

---

## Codex 条目来源

设定集优先写：势力、珠江新城地标、双身份规则、雨夜光学（项目自有术语，勿再用无意义英文占位）。UI 文案见 MacApp Codex。

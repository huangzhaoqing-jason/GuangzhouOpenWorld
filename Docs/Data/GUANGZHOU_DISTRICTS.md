# 广州城区与地标数据（统一表）

> **用途**：游戏切片与文档的**权威区划 ID**；统一旧代码里两套命名。  
> **来源**：公开地理与城市常识（珠江新城边界、天河地标等）；**不是**授权 3D 扫描资产，也不是 OSM 全量入库。  
> **相关**：[STORY_BIBLE.md](../STORY_BIBLE.md) · [TECH_STACK_FREE.md](../TECH_STACK_FREE.md) · `GZPCGGenerator` / `EGZCityDistrict`

---

## 1. 坐标基准

| 名称 | Lat / Lon（约） | 用途 |
|------|------------------|------|
| 全市配置中心（`DefaultEngine.ini` OSM） | **23.1291, 113.2644** | 偏越秀/旧城一带的全市锚；**不要**当成珠江新城中心 |
| **M3 切片中心（珠江新城）** | **23.118, 113.321**（约） | 花城广场—文化建筑群附近；任务与 CBD 切片原点意向 |
| 广州塔（对岸地标） | **23.106, 113.324**（约） | 江对岸视觉锚；幕 1 远景 |

数值为公开近似，落地时用 GDAL/OSM 精修；**不**把巨型 `.osm.pbf` 提交进 Git。

---

## 2. M3 切片：珠江新城（Zhujiang New Town）

公开规划边界意向（游戏用软边界，非测绘红线）：

| 边 | 参考 |
|----|------|
| 北 | 黄埔大道 |
| 南 | 珠江 |
| 西 | 广州大道 |
| 东 | 华南快速干线相关轴线 / 东侧快速路走廊 |

| 字段 | 值 |
|------|-----|
| `district_id` | `zhujiang_new_town` |
| 行政区隶属 | 天河区 |
| 游戏优先级 | **P0（幕 1 / M3）** |
| `EGZCityDistrict` | `TianheCBD` |
| PCG `EGZDistrictType` | `CBD`（模板名 Zhujiang New Town CBD） |

---

## 3. 统一区划表（权威 ID）

| district_id | 中文 | 优先级 | EGZCityDistrict | PCG 模板 / EGZDistrictType | 备注 |
|-------------|------|--------|-----------------|----------------------------|------|
| `zhujiang_new_town` | 珠江新城 / 天河 CBD | P0 | `TianheCBD` | CBD | 幕 1 |
| `yuexiu_old_town` | 越秀旧城 | P1 | `YuexiuOldTown` | OldTown | 幕 2 |
| `liwan_xiguan` | 荔湾西关 / 骑楼 | P1 | `LiwanXiguan` | Qilou | 与 PCG Shangxiajiu Qilou 关联 |
| `haizhu_waterfront` | 海珠滨江 | P2 | `HaizhuWaterfront` | Riverfront | 跨江 |
| `haizhu_innovation` | 海珠创新 / 琶洲意向 | P2 | `HaizhuInnovation` | — | 代码有枚举 |
| `baiyun_suburban` | 白云近郊 / 山体公园意向 | P3 | `BaiyunSuburban` | Park | PCG Baiyun Mountain Park |
| `panyu_university` | 番禺大学城 | P2 | `PanyuUniversity` | University | |
| `panyu_suburb` | 番禺郊区 / 工业意向 | P3 | `PanyuSuburb` | Industrial | 与 PCG Panyu Industrial 弱对应 |
| `huangpu_port` | 黄埔港区 | P3 | `HuangpuPort` | — | 幕 3 |
| `nansha_new_area` | 南沙新区 | P4 | `NanshaNewArea` | — | 远期 |
| `zhujiang_river` | 珠江水域带 | P1 | `ZhujiangRiver` | Riverfront | 跨区水域 |
| `tianhe_residential` | 天河居住区带 | P2 | （并入 Tianhe 周边） | Residential | 仅 PCG 有；暂不单开枚举 |

**权威规则**：新文档与 JSON 只用 `district_id`。C++ 枚举对齐留待有 UE 时改；本表为映射源。

---

## 4. 幕 1 / M3 地标锚点

| landmark_id | 名称 | 区划 | 剧情用途 |
|-------------|------|------|----------|
| `huacheng_square` | 花城广场 | zhujiang_new_town | 接单、竞速起点 |
| `opera_house` | 广州大剧院 | zhujiang_new_town | 双身份会面 |
| `guangdong_museum` | 广东省博物馆 | zhujiang_new_town | Side 监听 |
| `library` | 广州图书馆 | zhujiang_new_town | 收集物 |
| `childrens_palace` | 广州第二少年宫 | zhujiang_new_town | 可选氛围点 |
| `east_tower` | 东塔（CTF 等超高层意向） | zhujiang_new_town | 幕墙工单 |
| `west_tower` | 西塔 / IFC 意向 | zhujiang_new_town | 企业流明控股外景 |
| `pearl_river_front_axis` | 中轴江边段 | zhujiang_new_town | 「硬盘雨」目击 |
| `canton_tower` | 广州塔（江对岸） | zhujiang_river / 海珠侧 | 远景锚，非幕 1 可行走必须 |
| `liede_bridge` | 猎德大桥 | 跨江 | 通缉降温、跨区预告 |

资产获取：Blender 自建或 Megascans 通用材质；地标外形需合法授权后再做写实还原。

---

## 5. OSM / GDAL 指针（不入库大文件）

1. 从 Geofabrik 或其他 OSM 源下载广东/广州提取（本机或云，**勿提交 pbf**）。  
2. 用 **GDAL/OGR** 裁剪至珠江新城软边界 bbox（以 M3 中心 ± 约 1–2 km 起步）。  
3. 路网/建筑轮廓导出为 GeoJSON → 有 UE 后经 PCG / 导入流程进 `GZ_CBD_Slice`。  
4. 点云若用 **PDAL**，同样只存处理脚本与小样例，不存原始巨文件。

详见 [TECH_STACK_FREE.md](../TECH_STACK_FREE.md) 第七节。

---

## 6. 与配置的差异（诚实）

| 项 | 现状 |
|----|------|
| `OpenStreetMapSettings` 大半径 | 配置声明；无 OSMImporter 插件实体进仓 |
| PCG 八区 Bounds | 本地 cm 模板，**不是** WGS84 |
| 本文件 | 文档权威；游戏内尚未驱动 |

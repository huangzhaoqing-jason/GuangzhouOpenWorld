# M2 预备检查清单（有 UE5.8 的环境执行）

> **维护者本机无引擎时**：只维护本清单与约定，**不要**勾选「已通过」。  
> **执行环境**：安装 Unreal Engine **5.8** 的机器或云。  
> 相关：[ROADMAP_3A.md](ROADMAP_3A.md) · [ENGINE_STRATEGY.md](ENGINE_STRATEGY.md) · [TECH_STACK_FREE.md](TECH_STACK_FREE.md)

---

## Content 目录约定

| 路径（规划） | 里程碑 | 用途 |
|--------------|--------|------|
| `Content/Maps/GZ_BoxWorld` | **M2** | 最小盒子/平面世界，第三人称可走 |
| `Content/Maps/GZ_CBD_Slice` | **M3** | 珠江新城步行尺度切片 |
| `Content/Maps/GZ_MainWorld` | M4+ | 全市/多区；**当前缺失，勿当作已有** |

当前 [`Config/DefaultEngine.ini`](../Config/DefaultEngine.ini) 默认图仍指向：

```ini
GameDefaultMap=/Game/Maps/GZ_MainWorld.GZ_MainWorld
EditorStartupMap=/Game/Maps/GZ_MainWorld.GZ_MainWorld
```

**M1b / M2 第一步**：把上述两项临时改为 `GZ_BoxWorld`（或引擎 Empty 关卡），避免编辑器因缺图启动失败。

---

## M1b — 打开工程

- [ ] 用 UE5.8 打开 `GuangzhouOpenWorld.uproject`
- [ ] 确认 `JoltPhysics` / `SoLoud` 为 **Disabled**（Chaos + UE Audio）
- [ ] 无致命插件缺失报错（有警告可记录，不阻塞则继续）
- [ ] 能进入编辑器主界面

---

## M2 — 空白可玩

- [ ] 创建 `Content/Maps/GZ_BoxWorld`（平面 + 若干碰撞盒即可）
- [ ] 默认地图改指向 `GZ_BoxWorld`
- [ ] 第三人称角色可移动（引擎模板或现有 `GZCharacter`）
- [ ] 相机跟随正常
- [ ] 本地存档：保存一次 → 退出 PIE → 读档位置合理
- [ ] 录 30 秒操作视频归档（可选放 Issues / 网盘，不强制进 Git）

**不做（M2）**：天气全套、Mass 万人、载具打磨、联机、珠江新城美术。

---

## M3 预留（本清单不验收）

- [ ] 新建 `GZ_CBD_Slice`，地标与区划见 [Data/GUANGZHOU_DISTRICTS.md](Data/GUANGZHOU_DISTRICTS.md)
- [ ] 任务种子见 [Data/missions_act1.json](Data/missions_act1.json) 与 [STORY_BIBLE.md](STORY_BIBLE.md)

---

## 物理 / 音频抽检

- [ ] 角色站立与掉落使用 **Chaos**（非 Jolt）
- [ ] 基础碰撞盒可站立
- [ ] 主音量/环境音走 UE 内置（SoLoud 保持关）

---

## 状态

| 项 | 状态 |
|----|------|
| 清单文档本身 | 已入库 |
| 在 UE5.8 上勾选通过 | **未开始**（需有引擎环境） |

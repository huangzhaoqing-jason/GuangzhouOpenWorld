# M2 运行手册（有 UE5.8 的机器上执行）

> **本手册不能在无引擎的旧电脑上完成。** 云端 agent 无法替你点编辑器。  
> 精简勾选版见 [M2_PREP_CHECKLIST.md](M2_PREP_CHECKLIST.md)。  
> 引擎钉：UE **5.8**；物理默认 Chaos；Jolt/SoLoud 保持 Disabled。

---

## 0. 前置

| 需要 | 说明 |
|------|------|
| Apple Silicon Mac（或官方支持的 UE5.8 平台） | 按 [ENGINE_STRATEGY.md](ENGINE_STRATEGY.md) |
| Unreal Engine **5.8** 已安装 | Epic Launcher / 源码编 |
| 本仓库最新 `master` | 已含任务 JSON 加载器与 `Content/Data/missions_act1.json` |

```bash
git clone <repo-url> GuangzhouOpenWorld
cd GuangzhouOpenWorld
git checkout master
python3 Scripts/validate_mission_data.py   # 应打印 OK
```

---

## 1. 打开工程（M1b）

1. 双击或用 UE5.8 打开 `GuangzhouOpenWorld.uproject`。  
2. 若提示「引擎版本」，确认关联为 **5.8**。  
3. 打开 **Edit → Plugins**：确认 **JoltPhysics**、**SoLoud** 为 **未启用**。  
4. 无致命红色插件错误即可继续（警告可记下来，不阻塞则先走 M2）。

**失败常见原因**

| 现象 | 处理 |
|------|------|
| 找不到引擎 5.8 | 安装正式版 5.8，或暂改 `.uproject` 的 `EngineAssociation` 为你本机已装版本（并记入文档） |
| 第三方插件加载失败 | 保持 Jolt/SoLoud Disabled；勿强开无二进制的包装 |

---

## 2. 创建默认关卡 `GZ_BoxWorld`

配置已指向（见 `Config/DefaultEngine.ini`）：

```ini
GameDefaultMap=/Game/Maps/GZ_BoxWorld.GZ_BoxWorld
EditorStartupMap=/Game/Maps/GZ_BoxWorld.GZ_BoxWorld
```

**文件目前不存在**，首次打开可能报警。处理：

1. **File → New Level** → 选 Basic / Empty。  
2. 放一块大 `StaticMesh` 地板（或 BSP），加碰撞。  
3. **Save Current** → 路径 `Content/Maps/`，名 **`GZ_BoxWorld`**。  
4. 重启编辑器或 **File → Open Level** 确认能打开该图。  
5. 可选：放几个立方体障碍，方便测试碰撞。

---

## 3. 第三人称可走

任选其一：

- **快**：放入引擎 Third Person 模板角色，设为默认 Pawn / GameMode 覆盖。  
- **项目类**：使用 `GZCharacter` + `GZGameMode`（需在蓝图或 DefaultEngine 里指定 Pawn 类；若 PIE 无 Pawn，用模板先通关 M2）。

**PIE 验收**

1. Play → 能前后左右移动、镜头跟随。  
2. 不会无重力掉穿（地板有碰撞）。  

---

## 4. 确认幕 1 任务 JSON 已加载

1. 打开 **Window → Developer Tools → Output Log**。  
2. 过滤 `GZMission` / `GZGameInstance`。  
3. 期望类似：

```text
GZMissionJsonLoader: registered 8 missions from .../Content/Data/missions_act1.json
GZGameInstance: Act1 missions loaded = 8
```

| 若 registered = 0 | 检查 |
|-------------------|------|
| 缺文件 | `Content/Data/missions_act1.json` 是否在；可从 `Docs/Data/` 复制或跑 `Scripts/sync_missions_json.sh` |
| 解析失败 | 跑 `python3 Scripts/validate_mission_data.py` |

---

## 5. 存档往返

1. 在 PIE 中走到一个可辨认位置。  
2. 触发项目存档 API（`GZSaveGameManager`）或临时用控制台/蓝图 Save。  
3. 停 PIE → 再 PIE → Load，位置合理即过。  
4. （最低标准）若存档 UI 未接好：至少确认 `SaveManager` 对象在 GameInstance 中非空，并在 Log 打一次 Save 调用——完整读档可记为 M2 部分完成。

---

## 6. M2 完成定义（DoD）

- [ ] 编辑器无致命错误打开工程  
- [ ] `GZ_BoxWorld` 可进 PIE  
- [ ] 第三人称可走 ≥30 秒  
- [ ] Log 显示任务 JSON 注册成功（建议 ≥1）  
- [ ] 存档路径有一次成功尝试（完整读档更好）  

**不要在 M2 做**：珠江新城美术、Mass 万人、联机、天气全套。

下一里程碑：**M3** — `GZ_CBD_Slice` + [STORY_BIBLE.md](STORY_BIBLE.md) 幕 1 / [Data/missions_act1.json](Data/missions_act1.json)。

---

## 7. Apple Silicon 注意

- 用 **Mac Arm64** 目标；勿混 x86 插件。  
- Metal 相关报错先降画质 CVar，保 M2 可玩优先。  
- 打包 DMG 留给 M3 之后。

---

## 8. 仍然诚实

跑通本手册 = **空白可玩原型**，不是 3A 广州开放世界。进度口径见 [ROADMAP_3A.md](ROADMAP_3A.md) 与 [handoff/HANDOFF.md](handoff/HANDOFF.md)。

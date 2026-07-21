# [UI-Audit] 剧情 / 任务 Mock 扩展说明（非真机认证）

## Module
SwiftUI macOS 27 Liquid Glass shell — story & mission log mock (2026-07-21)

## 本轮变更
- Codex 文案对齐 `Docs/STORY_BIBLE.md` / 珠江新城区划
- 新增任务日志页，读取 Bundle 内 `missions_act1.json`（与 `Docs/Data/` 同源）
- HUD 增加通缉星级与当前任务目标 Mock
- `UE5CommandBridge` 增加 mission / wanted 命令与本地回放

## 认证状态
- **不是**真机 macOS 27 设备认证通过
- Linux headless 仍依赖 `#if os(macOS)` 隔离；本说明不替代 ui-audit-001/002/003 的历史评分
- 游戏仍不可玩；数据为设计稿 Mock

## 参考文档
- `Docs/STORY_BIBLE.md`
- `Docs/Data/GUANGZHOU_DISTRICTS.md`
- `Docs/Data/missions_act1.json`

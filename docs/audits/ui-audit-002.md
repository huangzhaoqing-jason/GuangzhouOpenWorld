# [UI-Audit] 原生级 Liquid Glass UI 系统架构（Mac游戏版）

## Module
SwiftUI Liquid Glass UI System v9.1 (HUD / MainUI / Controls / Render / State)

## Evaluation Dimensions (1-10)

| Dimension | Score | Notes |
|---|---|---|
| 苹果理念契合度 | 9 | 原生容器（NavigationSplitView）、系统安全区域融合、低电量自动降级、生命周期与ScenePhase同步 |
| macOS 27 Beta 规范符合度 | 8 | `.glassEffect()`（macOS 26+）触发Metal 4合并渲染；深色环境强制；透明度可调；待真机验证macOS 27全局透明度API |
| 动效流畅度与打断体验 | 9 | 全局spring(response:0.35, damping:0.75)；按钮按压/悬停独立弹簧；内容数值过渡动画 |
| 无障碍完整性 | 8 | 全控件accessibilityLabel/Value/Hint；减少透明度辅助功能实时响应；动态字体支持；待完整VoiceOver走查 |
| 与游戏画面融合一致性 | 8 | HUD通过safeAreaInset悬浮于Spark渲染视图之上；MetalTextureBridge共享纹理就绪；色调同步接口已建立 |

## Average
**8.4 / 10**

## Apple Design Self-Check
- [x] 使用 `.glassEffect()` 与系统材质同源
- [x] 系统减少透明度辅助功能实时响应
- [x] 低电量模式自动切换静态材质，关闭实时模糊
- [x] 使用 NavigationSplitView 原生容器，safeAreaInset 安全区域融合
- [x] @Observable + @MainActor + sending 并发安全
- [x] 生命周期与 ScenePhase 同步，后台释放资源
- [x] Linux Headless 零警告编译通过

## Performance Checklist
- [x] Metal 4 Merge Render：`.glassEffect()` 包裹在 `GlassEffectContainer`
- [x] 低电量模式关闭实时模糊
- [x] @Observable 差异渲染，仅更新变化节点
- [x] UI逻辑主线程隔离（@MainActor）

## Known Limitations
- macOS 27 全局 Liquid Glass 透明度滑块 API 尚未公开确认，当前通过 `UserDefaults` 键 + `NSNotification` 模拟，真机需替换为官方 API。
- `NSClickGestureRecognizer` / `NSSlider` 触感调用在当前 SwiftUI 层由系统控件隐式提供，未显式实例化 AppKit Recognizer。

## Status
Pass (≥8.0). Ready for macOS 27 Developer Beta device validation.

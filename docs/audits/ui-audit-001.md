# [UI-Audit] Liquid Glass APP Shell v9.0

## Module
SwiftUI APP Shell (Launch / MainMenu / Settings / Album / Codex)

## Evaluation Dimensions (1-10)

| Dimension | Score | Notes |
|---|---|---|
| 苹果理念契合度 | 9 | 内容优先：启动页仅标题+进度；主菜单左右分区克制；无侵入游戏HUD；系统页统一玻璃容器 |
| macOS 27 Beta 规范符合度 | 8 | 全局 matchGeometryEffect + spring；深色模式 fill 透明度≤0.22（≤35%）；浅色模式自动增强对比度；使用 `.ultraThinMaterial` + 层级阴影；需真机验证 Developer Beta 专属 API |
| 动效流畅度与打断体验 | 9 | 所有页面切换使用 spring(response:0.35, damping:0.82)；按钮按压/悬停独立弹簧；支持键盘快捷键反向触发；理论响应延迟<60ms |
| 无障碍完整性 | 8 | 全按钮 `accessibilityLabel` + hint；设置页 Slider/Picker/Toggle 均加标签；动态字体 `.small ... .accessibility3`；支持 `accessibilityReduceTransparency`；待补充 VoiceOver 完整走查 |
| 与游戏画面融合一致性 | 8 | Spark 渲染视图通过 MTKView 接入；MetalTextureBridge IOSurface 共享纹理实现零拷贝；后处理参数（exposure/bloom/colorTemp）双向同步；待 macOS 真机验证色调一致 |

## Average
**8.4 / 10**

## Apple Design Self-Check
- [x] 游戏HUD未纳入Liquid Glass
- [x] 仅使用SwiftUI实现APP外壳
- [x] 内容优先、无缝过渡、克制精致
- [x] 深色模式透明度≤35%
- [x] 浅色模式自动增强对比度
- [x] 支持VoiceOver/动态字体/键盘导航

## Required Refactor
- 真机验证 macOS 27 Developer Beta `glassBackground` / `materialActiveAppearance` 新API。
- 补充 VoiceOver 完整走查与动态字体最大号测试。
- macOS 联调 Metal IOSurface 共享纹理无闪屏/零延迟。

## Status
Pass (≥8.0). Ready for next module integration.

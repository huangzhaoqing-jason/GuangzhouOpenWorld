#if os(macOS)
import SwiftUI

// MARK: - 一行调用示例：将 Liquid Glass UI 系统集成至现有 Mac 游戏 View

/**
 使用方式：
 ```swift
 struct MyMacGameView: View {
     var body: some View {
         // 现有游戏渲染视图
         SparkRenderView()
             .ignoresSafeArea()
             // 一键集成 Liquid Glass HUD + 主UI 框架
             .liquidGlassGameUI()
     }
 }
 ```
 */
extension View {
    /// 一键集成：Liquid Glass 游戏 UI 系统（HUD + 主框架）
    /// 自动响应 macOS 27 全局 Liquid Glass 透明度滑块、低电量模式、Reduce Transparency / Increase Contrast
    func liquidGlassGameUI() -> some View {
        GameMainUI()
            .frame(minWidth: 1280, minHeight: 720)
    }
}

#endif

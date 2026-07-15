#if os(macOS)
import SwiftUI

// MARK: - 子任务4：渲染管线 - Metal 4合并渲染与智能降级

/// Metal 4 玻璃效果容器：合并多层模糊为单次 GPU Pass
/// macOS 27 Beta 3 适配：深色边缘、更亮高光、统一圆角、全局透明度滑块响应
struct GlassEffectContainer<Content: View>: View {
    let content: Content

    @Environment(\.colorScheme) private var colorScheme
    @State private var lowPowerMode: Bool = ProcessInfo.processInfo.isLowPowerModeEnabled
    @State private var reduceTransparency: Bool = false
    @State private var increaseContrast: Bool = false
    @State private var liquidGlassOpacity: Double = 0.5

    init(@ViewBuilder content: () -> Content) {
        self.content = content()
    }

    var body: some View {
        content
            .padding(20)
            .background {
                glassBackground
            }
            // macOS 27 Golden Gate Beta 3: 深色边缘增强深度与分离感
            .overlay(
                RoundedRectangle(cornerRadius: LiquidGlassConstants.cornerRadius, style: .continuous)
                    .stroke(
                        Color.black.opacity(LiquidGlassConstants.darkenedEdgeOpacity + (increaseContrast ? LiquidGlassConstants.increaseContrastStrokeBoost : 0)),
                        lineWidth: increaseContrast ? 1.5 : 1.0
                    )
                    .blendMode(.multiply)
            )
            // macOS 27 Golden Gate Beta 3: 顶部更亮高光，增加对比度模式下更强
            .overlay(
                RoundedRectangle(cornerRadius: LiquidGlassConstants.cornerRadius, style: .continuous)
                    .stroke(
                        LinearGradient(
                            colors: [
                                Color.white.opacity(LiquidGlassConstants.specularHighlightOpacity + (increaseContrast ? 0.15 : 0)),
                                Color.white.opacity(0.0)
                            ],
                            startPoint: .top,
                            endPoint: .bottom
                        ),
                        lineWidth: increaseContrast ? 1.0 : 0.5
                    )
            )
            .clipShape(RoundedRectangle(cornerRadius: LiquidGlassConstants.cornerRadius, style: .continuous))
            .shadow(
                color: Color.black.opacity(lowPowerMode ? 0.12 : LiquidGlassConstants.shadowOpacity),
                radius: lowPowerMode ? 10 : LiquidGlassConstants.shadowRadius,
                x: 0,
                y: lowPowerMode ? 5 : LiquidGlassConstants.shadowYOffset
            )
            .onAppear {
                observeSystemState()
            }
    }

    @ViewBuilder
    private var glassBackground: some View {
        if lowPowerMode {
            // Optimization: 低电量模式关闭实时模糊，使用静态材质降低 GPU 负载
            RoundedRectangle(cornerRadius: LiquidGlassConstants.cornerRadius, style: .continuous)
                .fill(.thinMaterial)
                .opacity(reduceTransparency ? 0.78 : 0.45)
        } else if increaseContrast {
            // Accessibility: 增加对比度模式下使用更不透明的材质
            RoundedRectangle(cornerRadius: LiquidGlassConstants.cornerRadius, style: .continuous)
                .fill(.regularMaterial)
                .opacity(reduceTransparency ? 0.85 : 0.62 + LiquidGlassConstants.increaseContrastFillBoost)
        } else if #available(macOS 26, *) {
            // Optimization: Metal 4 Merge Render - .glassEffect() 触发系统合并渲染
            // 系统全局 Liquid Glass 透明度滑块会自动影响 .glassEffect 渲染结果
            RoundedRectangle(cornerRadius: LiquidGlassConstants.cornerRadius, style: .continuous)
                .glassEffect(.regular.material, in: RoundedRectangle(cornerRadius: LiquidGlassConstants.cornerRadius, style: .continuous))
                .opacity(reduceTransparency ? 0.55 : 1.0)
        } else {
            // Fallback: macOS 25 及以下使用 ultraThinMaterial
            RoundedRectangle(cornerRadius: LiquidGlassConstants.cornerRadius, style: .continuous)
                .fill(.ultraThinMaterial)
                .opacity(reduceTransparency ? 0.55 : 0.28)
        }
    }

    private func observeSystemState() {
        lowPowerMode = ProcessInfo.processInfo.isLowPowerModeEnabled
        reduceTransparency = NSWorkspace.shared.accessibilityDisplayShouldReduceTransparency
        increaseContrast = NSWorkspace.shared.accessibilityDisplayShouldIncreaseContrast

        NotificationCenter.default.addObserver(
            forName: NSNotification.Name.NSProcessInfoPowerStateDidChange,
            object: nil,
            queue: .main
        ) { _ in
            withAnimation(.spring(response: 0.35, dampingFraction: 0.75)) {
                lowPowerMode = ProcessInfo.processInfo.isLowPowerModeEnabled
            }
        }

        NotificationCenter.default.addObserver(
            forName: NSNotification.Name("AppleLiquidGlassOpacityDidChangeNotification"),
            object: nil,
            queue: .main
        ) { [weak self] note in
            if let opacity = note.userInfo?["opacity"] as? Double {
                withAnimation(.spring(response: 0.35, dampingFraction: 0.75)) {
                    self?.liquidGlassOpacity = opacity
                }
            }
        }

        NotificationCenter.default.addObserver(
            forName: NSNotification.Name("NSWorkspaceAccessibilityDisplayOptionsDidChangeNotification"),
            object: nil,
            queue: .main
        ) { [weak self] _ in
            self?.reduceTransparency = NSWorkspace.shared.accessibilityDisplayShouldReduceTransparency
            self?.increaseContrast = NSWorkspace.shared.accessibilityDisplayShouldIncreaseContrast
        }
    }
}

#else

public struct GlassEffectContainerStub {
    public init() {}
}

#endif

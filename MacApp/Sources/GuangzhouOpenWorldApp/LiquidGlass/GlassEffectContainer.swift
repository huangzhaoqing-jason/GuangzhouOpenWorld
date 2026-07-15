#if os(macOS)
import SwiftUI

// MARK: - 子任务4：渲染管线 - Metal 4合并渲染与智能降级

/// Metal 4 玻璃效果容器：合并多层模糊为单次 GPU Pass
/// 低电量模式自动切换为静态材质，关闭实时模糊
struct GlassEffectContainer<Content: View>: View {
    let content: Content

    @Environment(\.colorScheme) private var colorScheme
    @State private var lowPowerMode: Bool = ProcessInfo.processInfo.isLowPowerModeEnabled
    @State private var reduceTransparency: Bool = false

    init(@ViewBuilder content: () -> Content) {
        self.content = content()
    }

    var body: some View {
        content
            .padding(20)
            .background {
                if lowPowerMode {
                    // Optimization: 低电量模式关闭实时模糊，使用静态材质降低 GPU 负载
                    RoundedRectangle(cornerRadius: 24, style: .continuous)
                        .fill(.thinMaterial)
                        .opacity(0.45)
                } else if #available(macOS 26, *) {
                    // Optimization: Metal 4 Merge Render - .glassEffect() 触发系统合并渲染
                    RoundedRectangle(cornerRadius: 24, style: .continuous)
                        .glassEffect(.regular.material, in: RoundedRectangle(cornerRadius: 24, style: .continuous))
                        .opacity(reduceTransparency ? 0.45 : 1.0)
                } else {
                    // Fallback: macOS 25 及以下使用 ultraThinMaterial
                    RoundedRectangle(cornerRadius: 24, style: .continuous)
                        .fill(.ultraThinMaterial)
                        .opacity(reduceTransparency ? 0.45 : 0.28)
                }
            }
            .overlay(
                RoundedRectangle(cornerRadius: 24, style: .continuous)
                    .stroke(Color.white.opacity(0.12), lineWidth: 0.5)
            )
            .clipShape(RoundedRectangle(cornerRadius: 24, style: .continuous))
            .shadow(
                color: Color.black.opacity(lowPowerMode ? 0.15 : 0.28),
                radius: lowPowerMode ? 12 : 28,
                x: 0,
                y: lowPowerMode ? 6 : 14
            )
            .onAppear {
                observeSystemState()
            }
    }

    private func observeSystemState() {
        lowPowerMode = ProcessInfo.processInfo.isLowPowerModeEnabled
        reduceTransparency = NSWorkspace.shared.accessibilityDisplayShouldReduceTransparency

        NotificationCenter.default.addObserver(
            forName: NSNotification.Name.NSProcessInfoPowerStateDidChange,
            object: nil,
            queue: .main
        ) { _ in
            withAnimation(.spring(response: 0.35, dampingFraction: 0.75)) {
                lowPowerMode = ProcessInfo.processInfo.isLowPowerModeEnabled
            }
        }
    }
}

#else

public struct GlassEffectContainerStub {
    public init() {}
}

#endif

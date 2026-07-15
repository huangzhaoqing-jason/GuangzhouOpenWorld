#if os(macOS)
import SwiftUI

// MARK: - 子任务1：HUD模块 - 系统级材质联动与可调性

/// 游戏HUD：与 macOS 27 Liquid Glass 系统材质联动
struct LiquidGlassHUD: View {
    @Bindable var state: GZUIState

    var body: some View {
        GlassEffectContainer {
            VStack(spacing: 16) {
                HUDTopBar(state: state)

                HStack(spacing: 12) {
                    ForEach(GZUIState.HUDTab.allCases) { tab in
                        HUDTabButton(tab: tab, state: state)
                    }
                }

                HUDPerformancePanel(state: state)
            }
            .padding(20)
        }
        // 强制深色环境以激活液态折射特性
        .environment(\.colorScheme, .dark)
        // 监听系统减少透明度辅助功能
        .environment(\.accessibilityReduceTransparency, state.reduceTransparency)
        .onAppear { state.startSystemObservation() }
    }
}

struct HUDTopBar: View {
    let state: GZUIState

    var body: some View {
        HStack {
            VStack(alignment: .leading, spacing: 4) {
                Text("Open广州")
                    .font(.headline.weight(.semibold))
                Text("天河CBD · 雨天")
                    .font(.caption)
                    .foregroundStyle(.secondary)
            }

            Spacer()

            HStack(spacing: 8) {
                Image(systemName: "wifi")
                Image(systemName: "battery.100")
            }
            .foregroundStyle(.secondary)
        }
    }
}

struct HUDTabButton: View {
    let tab: GZUIState.HUDTab
    let state: GZUIState

    @State private var isPressed: Bool = false

    var isSelected: Bool { state.selectedHUDTab == tab }

    var body: some View {
        Button(action: { state.selectedHUDTab = tab }) {
            Label(tab.rawValue, systemImage: tab.systemImage)
                .font(.caption.weight(.medium))
                .symbolRenderingMode(.hierarchical)
                .foregroundStyle(isSelected ? .primary : .secondary)
                .frame(maxWidth: .infinity)
                .padding(.vertical, 10)
                .background(
                    RoundedRectangle(cornerRadius: 12, style: .continuous)
                        .fill(isSelected ? Color.white.opacity(0.15) : Color.clear)
                )
                .scaleEffect(isPressed ? 0.94 : 1.0)
        }
        .buttonStyle(.plain)
        // 系统级悬停效果
        .hoverEffect(.highlight)
        .simultaneousGesture(
            DragGesture(minimumDistance: 0)
                .onChanged { _ in
                    withAnimation(.spring(response: 0.12, dampingFraction: 0.8)) {
                        isPressed = true
                    }
                }
                .onEnded { _ in
                    withAnimation(.spring(response: 0.12, dampingFraction: 0.8)) {
                        isPressed = false
                    }
                }
        )
        .accessibilityLabel(tab.rawValue)
        .accessibilityHint(isSelected ? "已选中" : "切换至\(tab.rawValue)")
    }
}

struct HUDPerformancePanel: View {
    let state: GZUIState

    var body: some View {
        HStack(spacing: 16) {
            VStack(alignment: .leading, spacing: 2) {
                Text("FPS")
                    .font(.caption2)
                    .foregroundStyle(.secondary)
                Text("\(Int(state.fps))")
                    .font(.system(.title2, design: .rounded, weight: .semibold))
                    .contentTransition(.numericText())
            }

            Divider()
                .frame(height: 32)

            VStack(alignment: .leading, spacing: 2) {
                Text("内存")
                    .font(.caption2)
                    .foregroundStyle(.secondary)
                Text("\(Int(state.memoryMB)) MB")
                    .font(.system(.title3, design: .rounded, weight: .semibold))
                    .contentTransition(.numericText())
            }

            Spacer()
        }
        .padding(12)
        .background(
            RoundedRectangle(cornerRadius: 14, style: .continuous)
                .fill(.ultraThinMaterial)
                .opacity(state.reduceTransparency ? 0.45 : state.liquidGlassOpacity)
        )
    }
}

// MARK: - 系统级联动扩展

extension GZUIState {
    /// 启动系统设置监听：低电量模式、减少透明度、全局玻璃透明度
    fileprivate func startSystemObservation() {
        self.lowPowerMode = ProcessInfo.processInfo.isLowPowerModeEnabled
        self.reduceTransparency = NSWorkspace.shared.accessibilityDisplayShouldReduceTransparency

        // 模拟 macOS 27 全局 Liquid Glass 透明度设置（UserDefaults 键）
        if let opacity = UserDefaults.standard.object(forKey: "com.apple.glassEffectGlobalOpacity") as? Double {
            self.liquidGlassOpacity = opacity
        }

        // 实际 macOS 27 可通过 NSNotificationCenter 监听系统设置变更
        NotificationCenter.default.addObserver(
            forName: NSNotification.Name("AppleGlassEffectOpacityDidChangeNotification"),
            object: nil,
            queue: .main
        ) { [weak self] note in
            if let opacity = note.userInfo?["opacity"] as? Double {
                self?.liquidGlassOpacity = opacity
            }
        }
    }
}

#else

public struct LiquidGlassHUDStub {
    public init() {}
}

#endif

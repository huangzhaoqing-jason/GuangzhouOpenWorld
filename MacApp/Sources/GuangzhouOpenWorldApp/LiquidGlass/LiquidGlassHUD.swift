#if os(macOS)
import SwiftUI

// MARK: - 子任务1：HUD模块 - 系统级材质联动与可调性

/// 游戏HUD：与 macOS 27 Beta 3 Liquid Glass 系统材质联动
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
        // 监听系统辅助功能：减少透明度 / 增加对比度
        .environment(\.accessibilityReduceTransparency, state.reduceTransparency)
        .environment(\.accessibilityIncreaseContrast, state.increaseContrast)
        .onAppear { state.startSystemObservation() }
    }
}

struct HUDTopBar: View {
    @Bindable var state: GZUIState

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
                Image(systemName: state.lowPowerMode ? "battery.25" : "battery.100")
            }
            .foregroundStyle(.secondary)
        }
    }
}

struct HUDTabButton: View {
    let tab: GZUIState.HUDTab
    @Bindable var state: GZUIState

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
                .scaleEffect(isPressed ? 0.94 : 1.0)
        }
        // macOS 27 Beta 3: 使用系统 .glass buttonStyle 获得原生 Liquid Glass 按钮
        .buttonStyle(.glass)
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
    @Bindable var state: GZUIState

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
                .opacity(state.effectiveGlassOpacity)
                .overlay(
                    RoundedRectangle(cornerRadius: 14, style: .continuous)
                        .stroke(Color.white.opacity(state.effectiveStrokeOpacity), lineWidth: 0.5)
                )
        )
    }
}

#else

public struct LiquidGlassHUDStub {
    public init() {}
}

#endif
